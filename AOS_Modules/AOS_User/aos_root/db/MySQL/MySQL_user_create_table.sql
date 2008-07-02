DROP TABLE IF EXISTS `aos`.`user`;
CREATE TABLE  `aos`.`user` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `username` varchar(64) NOT NULL,
  `password` varchar(80) NOT NULL,
  `data` text NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;