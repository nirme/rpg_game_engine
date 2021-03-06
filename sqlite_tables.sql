/*
RESOURCE_GRAPHIC
*/

CREATE TABLE `texture`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`name` VARCHAR(128) UNIQUE NOT NULL COLLATE BINARY,
	`filedata` LONGBLOB NOT NULL,
	`mip_levels` INT UNSIGNED,
	PRIMARY KEY (`id`)
);


CREATE TABLE `mesh`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`name` VARCHAR(128) UNIQUE NOT NULL COLLATE BINARY,
	`vertex_declaration` BLOB NOT NULL,
	`faces_count` INT UNSIGNED NOT NULL,
	`vertices_count` INT UNSIGNED NOT NULL,
	`vertices_data` LONGBLOB NOT NULL,
	`index_data` LONGBLOB NOT NULL,
	`bs_center_x` FLOAT NOT NULL,
	`bs_center_y` FLOAT NOT NULL,
	`bs_center_z` FLOAT NOT NULL,
	`bs_radius` FLOAT NOT NULL,
	PRIMARY KEY (`id`)
);

CREATE TABLE `attribute_info`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`mesh_id` INT UNSIGNED NOT NULL,
	`attrib_id` INT UNSIGNED NOT NULL,
	`face_start` INT UNSIGNED NOT NULL,
	`face_count` INT UNSIGNED NOT NULL,
	`vertex_start` INT UNSIGNED NOT NULL,
	`vertex_count` INT UNSIGNED NOT NULL,
	PRIMARY KEY (`id`)
);

CREATE TABLE `mesh_animation`
(
	`mesh_id` INT UNSIGNED UNIQUE NOT NULL,
	`animation_info_id` INT UNSIGNED NOT NULL,
	PRIMARY KEY (`mesh_id`)
);

CREATE TABLE `animation_info`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`bones_number` INT UNSIGNED NOT NULL,
	`frame_bone_data` LONGBLOB NOT NULL,
	`animation_data` LONGBLOB NOT NULL,
	PRIMARY KEY (`id`)
);

CREATE TABLE `effect`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`name` VARCHAR(128) UNIQUE NOT NULL COLLATE BINARY,
	`filedata` BLOB NOT NULL,
	PRIMARY KEY (`id`)
);

CREATE TABLE `effect_include`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`name` VARCHAR(128) UNIQUE NOT NULL COLLATE BINARY,
	`filedata` BLOB NOT NULL,
	PRIMARY KEY (`id`)
);


/*
RESOURCE_SOUND
*/

CREATE TABLE `sound`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`name` VARCHAR(128) UNIQUE NOT NULL COLLATE BINARY,
	`audiofile` LONGBLOB NOT NULL,
	`streamed` TINYINT(1) NOT NULL,
	`is3D` TINYINT(1) NOT NULL,
	`text` LONGBLOB,
	PRIMARY KEY (`id`)
);



/*
RESOURCE_OTHER
*/

/* GDI fonts description */
CREATE TABLE `gdi_font`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`name` VARCHAR(128) UNIQUE NOT NULL COLLATE BINARY,
	`font_file` MEDIUMBLOB NOT NULL,

	PRIMARY KEY (`id`)
);



/* Bitmap fonts description */
CREATE TABLE `bitmap_font`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`name` VARCHAR(128) UNIQUE NOT NULL COLLATE BINARY,
	`texture_id` VARCHAR(128) NOT NULL,

	`font_size` SMALLINT UNSIGNED NOT NULL,
	`line_height` SMALLINT UNSIGNED NOT NULL,
	`base` SMALLINT UNSIGNED NOT NULL,
	`bitmap_width` SMALLINT UNSIGNED NOT NULL,
	`bitmap_height` SMALLINT UNSIGNED NOT NULL,
	`mono_type_width` SMALLINT UNSIGNED NOT NULL,

	PRIMARY KEY (`id`)
);


CREATE TABLE `bitmap_character`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`font_id` INT UNSIGNED NOT NULL,

	`char_id` SMALLINT UNSIGNED NOT NULL,
	`tex_x` SMALLINT UNSIGNED NOT NULL,
	`tex_y` SMALLINT UNSIGNED NOT NULL,
	`tex_width` SMALLINT UNSIGNED NOT NULL,
	`tex_height` SMALLINT UNSIGNED NOT NULL,
	`kerning` BLOB,
	/* array of pairs (USHORT nextCharId, short kerningLength) number of elements equals = `kerning`.size / (short.size * 2) */

	PRIMARY KEY (`id`)
);


/* LIGHT */
CREATE TABLE `light`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`name` VARCHAR(128) UNIQUE NOT NULL COLLATE BINARY,

	`light_type` TINYINT UNSIGNED NOT NULL,
	`priority` TINYINT UNSIGNED NOT NULL,
	`generate_shadows` TINYINT(1) NOT NULL,

	`color_difuse` INT UNSIGNED NOT NULL,
	`color_specular` INT UNSIGNED NOT NULL,
	`range` FLOAT NOT NULL,
	`attenuation0` FLOAT NOT NULL,
	`attenuation1` FLOAT NOT NULL,
	`attenuation2` FLOAT NOT NULL,
	`hotspot_angle` FLOAT NOT NULL,
	`cutoff_angle` FLOAT NOT NULL,
	`falloff` FLOAT NOT NULL,

	PRIMARY KEY (`id`) 
);


/* PARTICLE SYSTEM */

CREATE TABLE `particle_system`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`name` VARCHAR(128) UNIQUE NOT NULL COLLATE BINARY,

	`texture_id` VARCHAR(128) NOT NULL,
	`sprites_number` SMALLINT UNSIGNED NOT NULL,
	`sprites_columns` SMALLINT UNSIGNED NOT NULL,
	`sprites_rows` SMALLINT UNSIGNED NOT NULL,

	`emissiveFactor` FLOAT NOT NULL,

	`max_particles_number` SMALLINT UNSIGNED NOT NULL,
	`particleSorting` TINYINT UNSIGNED NOT NULL,
	/* NONE 0, PARTIAL 1, FULL 2 */

	`billboarding_type` TINYINT UNSIGNED NOT NULL,
	/* NONE 0, VIEWPLANE 1, VIEWPOINT 2 */

	`particle_batches_number` SMALLINT UNSIGNED NOT NULL,

	`draw_only_particles_before_camera` TINYINT(1) UNSIGNED NOT NULL,

	`particle_processor_type` SMALLINT UNSIGNED NOT NULL,
	`particle_processor_data` BLOB NOT NULL,

	PRIMARY KEY (`id`) 
);


/* GUI */


CREATE TABLE `GUI_Element`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`type` INT UNSIGNED NOT NULL,
	`resource_id` VARCHAR(128) NOT NULL,
	`data` BLOB,
/*
type 1 (GUIElementTexture):
{
	RECT textureCoord;
	DWORD color;
}

type 2 (GUIElementText):
{
	INT fontHeightInPoints; (-1 for default)
	DWORD textFormat;
	DWORD textColor;
	UINT textLength;
	wchar_t[] text;
	INT textLenghtShown;
}

type 3 (GUIElementBitmapText):
{
	float textScale;
	DWORD textFormat;
	DWORD textColor;
	UINT textLength;
	wchar_t[] text;
	INT textLenghtShown;
}
*/

	PRIMARY KEY (`id`)
);


CREATE TABLE `GUI_Control`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`type` INT UNSIGNED NOT NULL,
	`position_left` INT,
	`position_top` INT,
	`position_right` INT,
	`position_bottom` INT,
	`hotkey` INT UNSIGNED,
	`additional_data` BLOB,
/*
type 1 (GUI_CONTROL_STATIC):
	NULL
type 2 (GUI_CONTROL_BUTTON):
	int[] states
		/*GUI_CONTROL_BUTTON_STATE_FREE, GUI_CONTROL_BUTTON_STATE_ACTIVE, GUI_CONTROL_BUTTON_STATE_DOWN, GUI_CONTROL_BUTTON_STATE_DISABLED*/
/*
	{    elements, indices,    elements, indices,    elements, indices,    elements, indices    }
*/

	PRIMARY KEY (`id`)
);


CREATE TABLE `GUI_Dialog`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`position_left` INT,
	`position_top` INT,
	`position_right` INT,
	`position_bottom` INT,
	`background_element_texture_id` INT UNSIGNED,

	PRIMARY KEY (`id`)
);


CREATE TABLE `GUI_Panel`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`panel_name` VARCHAR(128) UNIQUE NOT NULL,
	`background_element_texture_id` INT UNSIGNED,

	PRIMARY KEY (`id`)
);


CREATE TABLE `GUI_ScreenSize_Id_Bits`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`screen_width` INT UNSIGNED NOT NULL,
	`screen_aspect` FLOAT NOT NULL,
	`bit_flag` INT UNSIGNED NOT NULL,

	PRIMARY KEY (`id`)
);


CREATE TABLE `GUI_System_Event`
(
	`control_id` INT UNSIGNED UNIQUE NOT NULL,
	`push_panel` TINYINT(1),
	`show_previous_panel` TINYINT(1),
	`panel_to_show` VARCHAR(128),
	
	PRIMARY KEY (`control_id`)
);



CREATE TABLE `GUI_Control_Elements`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`control_id` INT UNSIGNED NOT NULL,
	`element_id` INT UNSIGNED NOT NULL,

	PRIMARY KEY (`id`)
);


CREATE TABLE `GUI_Dialog_Controls`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`dialog_id` INT UNSIGNED NOT NULL,
	`control_id` INT UNSIGNED NOT NULL,

	PRIMARY KEY (`id`)
);


CREATE TABLE `GUI_Panel_Dialogs`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`panel_id` INT UNSIGNED NOT NULL,
	`dialog_id` INT UNSIGNED NOT NULL,

	PRIMARY KEY (`id`)
);


CREATE TABLE `GUI_System_Panels`
(
	`id` INT UNSIGNED UNIQUE NOT NULL,
	`system_id` INT UNSIGNED,
	`panel_id` INT UNSIGNED NOT NULL,

	PRIMARY KEY (`id`)
);







/*
CREATE TABLE ``
(
	`mesh_id` INT UNSIGNED UNIQUE NOT NULL,
	`type` SMALLINT UNSIGNED NOT NULL,
	`data` BLOB,
	PRIMARY KEY (`mesh_id`)
);

*/








/*CHECK AND SET CORRECTLY*/
CREATE TABLE `bounding_volume_info`
(
	`mesh_id` INT UNSIGNED UNIQUE NOT NULL,
	`type` SMALLINT UNSIGNED NOT NULL,
	`data` BLOB,
	PRIMARY KEY (`mesh_id`)
);


















CREATE TABLE `sg_node`
(
	`id`			INT UNSIGNED UNIQUE NOT NULL,
	`parent_id`		INT UNSIGNED,
	`name`			VARCHAR(128) NOT NULL,
	`class_id`		INT UNSIGNED NOT NULL,
	`class_data`	LONGBLOB NOT NULL,
	PRIMARY KEY (`id`)
);


CREATE TABLE `graphic_object`
(
	`id`					INT UNSIGNED UNIQUE NOT NULL,
	`name`					VARCHAR(128) NOT NULL,
	`mesh_id`				INT UNSIGNED NOT NULL,
	`default_render_pass`	INT UNSIGNED NOT NULL,
	PRIMARY KEY (`id`)
);

CREATE TABLE `graphi_object_chunk`
(
	`id`							INT UNSIGNED UNIQUE NOT NULL,
	`graphic_object_id`				INT UNSIGNED NOT NULL,
	`subset_number`					INT UNSIGNED NOT NULL,
	`graphi_object_material_index`	INT UNSIGNED NOT NULL,
	PRIMARY KEY (`id`)
);


CREATE TABLE `graphic_object_material`
(
	`id`				INT UNSIGNED UNIQUE NOT NULL,
	`graphic_object_id`	INT UNSIGNED NOT NULL,
	`material_index`	INT UNSIGNED NOT NULL,
	`effect_id`			INT UNSIGNED NOT NULL,
	`is_transparent`	TINYINT(1) NOT NULL,
	PRIMARY KEY (`id`)
);

CREATE TABLE `material_parameter`
(
	`id`				INT UNSIGNED UNIQUE NOT NULL,
	`material_id`		INT UNSIGNED NOT NULL,
	`parameter_name`	VARCHAR(128) NOT NULL,
	`parameter_value`	LONGBLOB NOT NULL,
	PRIMARY KEY (`id`)
);

CREATE TABLE `material_texture`
(
	`id`						INT UNSIGNED UNIQUE NOT NULL,
	`material_id`				INT UNSIGNED NOT NULL,
	`texture_parameter_name`	VARCHAR(128) NOT NULL,
	`texture_id`				INT UNSIGNED NOT NULL,
	PRIMARY KEY (`id`)
);



