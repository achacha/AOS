DROP TABLE IF EXISTS `aos`.`global`;
CREATE TABLE  `aos`.`global` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `name` varchar(45) character set latin1 NOT NULL,
  `value` varchar(2048) character set latin1 NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;