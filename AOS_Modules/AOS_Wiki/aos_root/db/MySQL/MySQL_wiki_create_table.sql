DROP TABLE IF EXISTS `aos`.`wiki`;
CREATE TABLE  `aos`.`wiki` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `path` varchar(1024) NOT NULL,
  `data` text character set utf8 collate utf8_bin NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;