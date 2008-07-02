DROP TABLE IF EXISTS `aos`.`session`;
CREATE TABLE  `aos`.`session` (
  `id` varchar(48) NOT NULL,
  `data` text NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;