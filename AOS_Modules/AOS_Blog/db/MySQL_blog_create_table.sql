DROP TABLE IF EXISTS `aos`.`blog`;
CREATE TABLE  `aos`.`blog` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `user_id` int(10) unsigned NOT NULL COMMENT 'user.id',
  `title` varchar(1024) character set latin1 NOT NULL,
  `data` text character set latin1 NOT NULL,
  `created` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  PRIMARY KEY  (`id`),
  CONSTRAINT `user_id` FOREIGN KEY (`id`) REFERENCES `user` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;