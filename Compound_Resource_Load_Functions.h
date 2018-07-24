#include "Loader_v2.h"




////////////////////////////////////////////////////
////////           GRAPHIC_OBJECT           ////////
////////////////////////////////////////////////////

int Loader::resourceLoadGraphicObject(sqlite3* _db, void* _ptr, void* _aData)
{
	GraphicObject* pGraphicObject = static_cast <GraphicObject*>(_ptr);
	if (pGraphicObject->isSet())
		return 0;

	if (pGraphicObject->getDeletionFlag())
	{
		pGraphicObject->setDeletionFlag(false);
		pGraphicObject->release();
	}

	int result = 0;

	string pGO_name = pGraphicObject->getName();
	int pGO_dwRenderPass = 0;
	Mesh* pGO_pMesh = NULL;
	vector<Material*> pGO_v_pMaterial;
	vector<RenderChunk*> pGO_renderChunks;



/*
+---+---------------------+
|col| graphic_object      |
+---+---------------------+
| 0 | id                  |
| 1 | mesh_id             |
| 2 | default_render_pass |
+---+---------------------+
*/
	sqlite3_stmt* stmt = NULL;
	ostringstream sql;

	sql << "SELECT `id`, `mesh_id`, `default_render_pass` FROM `graphic_object` WHERE `name` = '" << pGO_name << "'";
	result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL);

	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK ||
		((result = sqlite3_step(stmt)) != SQLITE_ROW && result != SQLITE_DONE))
	{
		sqlite3_finalize(stmt);
		showError(true, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		return 1;
	}

	UINT graphicObjectId = (UINT) sqlite3_column_int(stmt, 0);
	UINT meshId = (UINT) sqlite3_column_int(stmt, 1);
	pGO_pMesh = Mesh::getResource(meshId);
	pGO_dwRenderPass = sqlite3_column_int(stmt, 2);

	sqlite3_finalize(stmt);
	sql.clear();


/*
+---+-------------------------+
|col| graphic_object_material |
+---+-------------------------+
| 0	| id                      |
| 1	| material_index          |
| 2 | effect_id               |
| 3 | is_transparent          |
+---+-------------------------+
*/
	sql << "SELECT `id`, `material_index`, `effect_id`, `is_transparent` FROM `graphic_object_material` WHERE `graphic_object_id` = " << graphicObjectId ;
	result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL);
	if (result != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		showError(true, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		return 1;
	}

	map<UINT /*index*/, Material*> m_id_materials;
	map<UINT /*index*/, Material*> m_ind_materials;
	UINT size = 0;
	while (result = sqlite3_step(stmt) == SQLITE_ROW)
	{
		Material* mat = new Material;
		Effect* eff = Effect::getResource(sqlite3_column_int(stmt, 2));
		mat->setEffect(eff);
		mat->setTransparent(sqlite3_column_int(stmt, 3) != 0 ? true : false);

		m_id_materials.insert(map<UINT, Material*>::value_type(sqlite3_column_int(stmt, 0), mat));
		m_ind_materials.insert(map<UINT, Material*>::value_type(sqlite3_column_int(stmt, 1), mat));

		size = max(size, (UINT )sqlite3_column_int(stmt, 1));
	}
	if (result == SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		showError(true, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		return 2;
	}
	sqlite3_finalize(stmt);
	sql.clear();

	
	for (map<UINT, Material*>::iterator it = m_id_materials.begin(); it != m_id_materials.end(); it++)
	{
/*
+---+--------------------+
|col| material_parameter |
+---+--------------------+
| 0	| parameter_name     |
| 1 | parameter_value    |
+---+--------------------+
*/
		sql << "SELECT `parameter_name`, `parameter_value` FROM `material_parameter` WHERE `material_id` = " << (*it).first;
		result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL);
		if (result != SQLITE_OK)
		{
			sqlite3_finalize(stmt);
			showError(true, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
			return 1;
		}

		while (result = sqlite3_step(stmt) == SQLITE_ROW)
		{
			(*it).second->setEffectParameter((char*)sqlite3_column_text(stmt,0), sqlite3_column_blob(stmt,1), sqlite3_column_bytes(stmt,1));
		}
		if (result == SQLITE_DONE)
		{
			sqlite3_finalize(stmt);
			showError(true, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
			return 2;
		}
		sqlite3_finalize(stmt);
		sql.clear();


/*
+---+------------------------+
|col| material_texture       |
+---+------------------------+
| 0	| texture_parameter_name |
| 1 | texture_id             |
+---+------------------------+
*/
		sql << "SELECT `texture_parameter_name`, `texture_id` FROM `material_texture` WHERE `material_id` = " << (*it).first;
		result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL);
		if (result != SQLITE_OK)
		{
			sqlite3_finalize(stmt);
			showError(true, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
			return 1;
		}

		while (result = sqlite3_step(stmt) == SQLITE_ROW)
		{
			Texture* tex = Texture::getResource((UINT)sqlite3_column_int(stmt,1));
			(*it).second->setTexture((char*)sqlite3_column_text(stmt,0), tex);
		}
		if (result == SQLITE_DONE)
		{
			sqlite3_finalize(stmt);
			showError(true, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
			return 2;
		}
		sqlite3_finalize(stmt);
		sql.clear();
	}

	
	pGO_v_pMaterial.resize(size);
	for (UINT i=0;i<size;++i)
	{
		map<UINT,Material*>::iterator it = m_ind_materials.find(i);
		if (it != m_ind_materials.end())
			pGO_v_pMaterial[i] = (*it).second;
		else
			pGO_v_pMaterial[i] = NULL;
	}
	m_ind_materials.clear();
	m_id_materials.clear();


	sql << "SELECT COUNT(*) AS `number` FROM `graphi_object_chunk` WHERE `graphic_object_id` = " << graphicObjectId;
	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK || 
		(result = sqlite3_step(stmt)) != SQLITE_ROW)
	{
		sqlite3_finalize(stmt);
		showError(true, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		return 1;
	}

	UINT renderChunksNumber = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);
	sql.clear();

/*
+---+-------------------------------+
|col| graphic_object_chunk          |
+---+-------------------------------+
| 0	| subset_number                 |
| 1 | graphic_object_material_index |
+---+-------------------------------+
*/
	sql << "SELECT `subset_number`, `graphic_object_material_index` FROM `graphi_object_chunk` WHERE `graphic_object_id` = " << graphicObjectId;
	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK ||
		((result = sqlite3_step(stmt)) != SQLITE_ROW && result != SQLITE_DONE))
	{
		sqlite3_finalize(stmt);
		showError(true, "sqlite3 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		return 1;
	}

	pGO_renderChunks.resize(renderChunksNumber);
	UINT i=0;
	while ((result = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		pGO_renderChunks[i] = new RenderChunk(pGraphicObject, sqlite3_column_int(stmt, 0), sqlite3_column_int(stmt, 1));
		i++;
	}
	if (result != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		showError(true, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		return 1;
	}


	pGraphicObject->setResources(pGO_pMesh, &pGO_v_pMaterial, pGO_dwRenderPass, &pGO_renderChunks);

	if (pGraphicObject->getDeletionFlag())
	{
		pGraphicObject->setDeletionFlag(false);
		pGraphicObject->release();
	}

	pGraphicObject->isLoaded();

	return 0;
};

