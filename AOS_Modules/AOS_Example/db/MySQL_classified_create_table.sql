DROP TABLE IF EXISTS `aos`.`classified`;
CREATE TABLE  `aos`.`classified` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `title` varchar(1024) character set latin1 NOT NULL,
  `descript` text character set latin1 NOT NULL,
  `contact` varchar(256) character set latin1 NOT NULL,
  `price` varchar(64) character set latin1 NOT NULL,
  `user_id` int(10) unsigned NOT NULL COMMENT 'user.id',
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;