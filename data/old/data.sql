CREATE TABLE `WorldObject` (
`id` INTEGER PRIMARY KEY AUTOINCREMENT,
`vertex_data` LONGBLOB NOT NULL,
`face_data` LONGBLOB NOT NULL,
`texture` LONGBLOB NOT NULL,
`material` LONGBLOB NOT NULL,
`model_name` VARCHAR(32) NOT NULL,
`object_name` VARCHAR(255) NOT NULL,
`type` BLOB(4) NOT NULL
);

CREATE TABLE `OtherData` (
`id` INTEGER PRIMARY KEY AUTOINCREMENT,
`data` LONGBLOB NOT NULL,
`data_name` VARCHAR(255) NOT NULL
);

`position` BLOB(64) NOT NULL,
