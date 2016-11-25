/*
Navicat MySQL Data Transfer

Source Server         : 42.62.125.190
Source Server Version : 50550
Source Host           : 42.62.125.190:13579
Source Database       : mario

Target Server Type    : MYSQL
Target Server Version : 50550
File Encoding         : 65001

Date: 2016-11-23 21:48:40
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for mr_host
-- ----------------------------
DROP TABLE IF EXISTS `mr_host`;
CREATE TABLE `mr_host` (
  `host_id` bigint(20) NOT NULL COMMENT '主机ID',
  `host` varchar(128) NOT NULL COMMENT '主机',
  `name` varchar(128) NOT NULL COMMENT '名称',
  `ip` varchar(15) NOT NULL COMMENT 'IP地址',
  `minion_id` varchar(32) NOT NULL COMMENT 'MINION ID',
  `desc` varchar(255) DEFAULT null COMMENT '描述',
  PRIMARY KEY (`host_id`),
  UNIQUE KEY `unique_minion` (`minion_id`),
  KEY `ip` (`ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of mr_host
-- ----------------------------
INSERT INTO `mr_host` VALUES ('0', '全部主机', '全部主机', '\'\'', '*', null);
INSERT INTO `mr_host` VALUES ('1', '不指定主机', '不指定主机', '\'\'', '---', null);
INSERT INTO `mr_host` VALUES ('10552', 'NEW-7-1-4-1', 'NEW-7-1-4-1', '10.10.107.107', 'new080027A2A128', null);
INSERT INTO `mr_host` VALUES ('10553', 'NEW-5-1-2-1', 'NEW-5-1-2-1', '10.10.105.103', 'new080027DF3D9D', null);
INSERT INTO `mr_host` VALUES ('10557', 'NEW-7-5-4-1', 'NEW-7-5-4-1', '10.10.107.140', 'new08002703D960', null);
INSERT INTO `mr_host` VALUES ('10558', 'NEW-5-2-3-1', 'NEW-5-2-3-1', '10.10.105.114', 'new080027860204', null);
INSERT INTO `mr_host` VALUES ('10563', 'NEW-5-1-3-1', 'NEW-5-1-3-1', '10.10.105.106', 'new0800270F1B1F', null);
INSERT INTO `mr_host` VALUES ('10564', 'NEW-5-4-3-1', 'NEW-5-4-3-1', '10.10.105.129', 'new0800270E808E', null);
INSERT INTO `mr_host` VALUES ('10565', 'NEW-5-1-3-2', 'NEW-5-1-3-2', '10.10.105.105', 'new080027ADF439', null);
INSERT INTO `mr_host` VALUES ('10568', 'NEW-5-4-2-1', 'NEW-5-4-2-1', '10.10.105.128', 'new080027817B05', null);
INSERT INTO `mr_host` VALUES ('10570', 'NEW-5-5-3-1', 'NEW-5-5-3-1', '10.10.105.138', 'new080027955089', null);
INSERT INTO `mr_host` VALUES ('10573', 'NEW-5-5-2-1', 'NEW-5-5-2-1', '10.10.105.136', 'new080027F31340', null);
INSERT INTO `mr_host` VALUES ('10577', 'NEW-5-3-4-1', 'NEW-5-3-4-1', '10.10.105.123', 'new08002734BD44', null);
INSERT INTO `mr_host` VALUES ('10578', 'NEW-5-2-3-2', 'NEW-5-2-3-2', '10.10.105.113', 'new080027051576', null);
INSERT INTO `mr_host` VALUES ('10584', 'NEW-5-3-3-2', 'NEW-5-3-3-2', '10.10.105.122', 'new080027C22850', null);
INSERT INTO `mr_host` VALUES ('10586', 'NEW-5-5-2-2', 'NEW-5-5-2-2', '10.10.105.135', 'new080027148D9D', null);
INSERT INTO `mr_host` VALUES ('10592', 'NEW-5-2-4-2', 'NEW-5-2-4-2', '10.10.105.115', 'new0800275A2A40', null);
INSERT INTO `mr_host` VALUES ('10596', 'NEW-5-1-4-2', 'NEW-5-1-4-2', '10.10.105.107', 'new080027186629', null);
INSERT INTO `mr_host` VALUES ('10597', 'NEW-5-1-1-1', 'NEW-5-1-1-1', '10.10.105.102', 'new080027026A50', null);
INSERT INTO `mr_host` VALUES ('10598', 'NEW-5-1-2-2', 'NEW-5-1-2-2', '10.10.105.104', 'new080027E631D3', null);
INSERT INTO `mr_host` VALUES ('10599', 'NEW-5-5-3-2', 'NEW-5-5-3-2', '10.10.105.137', 'new0800273C1D97', null);
INSERT INTO `mr_host` VALUES ('10600', 'NEW-5-2-2-2', 'NEW-5-2-2-2', '10.10.105.112', 'new08002790058E', null);
INSERT INTO `mr_host` VALUES ('10601', 'NEW-5-1-4-1', 'NEW-5-1-4-1', '10.10.105.108', 'new080027B5F82D', null);
INSERT INTO `mr_host` VALUES ('10602', 'NEW-5-5-1-2', 'NEW-5-5-1-2', '10.10.105.134', 'new080027FE808C', null);
INSERT INTO `mr_host` VALUES ('10604', 'NEW-5-3-4-2', 'NEW-5-3-4-2', '10.10.105.124', 'new08002775CB70', null);
INSERT INTO `mr_host` VALUES ('10608', 'NEW-5-1-1-2', 'NEW-5-1-1-2', '10.10.105.101', 'new080027A6E229', null);
INSERT INTO `mr_host` VALUES ('10611', 'NEW-5-3-1-2', 'NEW-5-3-1-2', '10.10.105.117', 'new080027D9EB92', null);
INSERT INTO `mr_host` VALUES ('10613', 'NEW-5-5-1-1', 'NEW-5-5-1-1', '10.10.105.133', 'new0800275F127A', null);
INSERT INTO `mr_host` VALUES ('10615', 'NEW-5-5-4-1', 'NEW-5-5-4-1', '10.10.105.139', 'new080027AC9609', null);
INSERT INTO `mr_host` VALUES ('10619', 'NEW-5-4-4-1', 'NEW-5-4-4-1', '10.10.105.131', 'new0800277812D2', null);
INSERT INTO `mr_host` VALUES ('10620', 'NEW-5-5-4-2', 'NEW-5-5-4-2', '10.10.105.140', 'new08002761163E', null);
INSERT INTO `mr_host` VALUES ('10622', 'NEW-5-4-1-1', 'NEW-5-4-1-1', '10.10.105.126', 'new0800278C122E', null);
INSERT INTO `mr_host` VALUES ('10625', 'NEW-5-3-2-2', 'NEW-5-3-2-2', '10.10.105.119', 'new0800276DB869', null);
INSERT INTO `mr_host` VALUES ('10628', 'NEW-5-3-1-1', 'NEW-5-3-1-1', '10.10.105.118', 'new080027718E04', null);
INSERT INTO `mr_host` VALUES ('10630', 'NEW-5-2-2-1', 'NEW-5-2-2-1', '10.10.105.111', 'new080027EF53F6', null);
INSERT INTO `mr_host` VALUES ('10631', 'NEW-5-4-2-2', 'NEW-5-4-2-2', '10.10.105.127', 'new080027763E36', null);
INSERT INTO `mr_host` VALUES ('10634', 'NEW-5-2-1-1', 'NEW-5-2-1-1', '10.10.105.110', 'new08002705D9EC', null);
INSERT INTO `mr_host` VALUES ('10638', 'NEW-5-4-3-2', 'NEW-5-4-3-2', '10.10.105.130', 'new08002709E6AE', null);
INSERT INTO `mr_host` VALUES ('10642', 'NEW-5-3-2-1', 'NEW-5-3-2-1', '10.10.105.120', 'new080027F20221', null);
INSERT INTO `mr_host` VALUES ('10643', 'NEW-5-3-3-1', 'NEW-5-3-3-1', '10.10.105.121', 'new0800279EE5F8', null);
INSERT INTO `mr_host` VALUES ('10645', 'NEW-5-4-4-2', 'NEW-5-4-4-2', '10.10.105.132', 'new0800277E6CF0', null);
INSERT INTO `mr_host` VALUES ('10649', 'NEW-5-2-1-2', 'NEW-5-2-1-2', '10.10.105.109', 'new0800271D1C53', null);
INSERT INTO `mr_host` VALUES ('10662', 'NEW-5-2-4-1', 'NEW-5-2-4-1', '10.10.105.116', 'new08002709EAA1', null);
INSERT INTO `mr_host` VALUES ('10726', 'NEW-7-1-4-2', 'NEW-7-1-4-2', '10.10.107.108', 'new0800270795D5', null);
INSERT INTO `mr_host` VALUES ('10727', 'NEW-7-1-1-1', 'NEW-7-1-1-1', '10.10.107.101', 'new0800270C00F4', null);
INSERT INTO `mr_host` VALUES ('10728', 'NEW-7-4-2-2', 'NEW-7-4-2-2', '10.10.107.128', 'new080027983D24', null);
INSERT INTO `mr_host` VALUES ('10729', 'NEW-7-5-1-1', 'NEW-7-5-1-1', '10.10.107.134', 'new080027A12E30', null);
INSERT INTO `mr_host` VALUES ('10730', 'NEW-7-4-4-2', 'NEW-7-4-4-2', '10.10.107.132', 'new080027C252A8', null);
INSERT INTO `mr_host` VALUES ('10731', 'NEW-7-2-3-2', 'NEW-7-2-3-2', '10.10.107.114', 'new08002713E8D7', null);
INSERT INTO `mr_host` VALUES ('10733', 'NEW-7-5-2-1', 'NEW-7-5-2-1', '10.10.107.135', 'new0800278F78BB', null);
INSERT INTO `mr_host` VALUES ('10745', 'NEW-7-5-1-2', 'NEW-7-5-1-2', '10.10.107.133', 'new0800277D938D', null);
INSERT INTO `mr_host` VALUES ('10746', 'NEW-7-5-3-2', 'NEW-7-5-3-2', '10.10.107.138', 'new0800278988EC', null);
INSERT INTO `mr_host` VALUES ('10758', 'NEW-7-3-4-2', 'NEW-7-3-4-2', '10.10.107.124', 'new080027D63549', null);
INSERT INTO `mr_host` VALUES ('10762', 'NEW-7-4-3-1', 'NEW-7-4-3-1', '10.10.107.129', 'new08002719EB54', null);
INSERT INTO `mr_host` VALUES ('10766', 'NEW-7-4-3-2', 'NEW-7-4-3-2', '10.10.107.130', 'new080027ADC674', null);
INSERT INTO `mr_host` VALUES ('10768', 'NEW-7-3-2-1', 'NEW-7-3-2-1', '10.10.107.120', 'new08002781425D', null);
INSERT INTO `mr_host` VALUES ('10770', 'NEW-7-2-4-2', 'NEW-7-2-4-2', '10.10.107.115', 'new08002750887B', null);
INSERT INTO `mr_host` VALUES ('10783', 'NEW-7-4-1-2', 'NEW-7-4-1-2', '10.10.107.125', 'new080027D494C3', null);
INSERT INTO `mr_host` VALUES ('10786', 'NEW-7-1-2-1', 'NEW-7-1-2-1', '10.10.107.103', 'new080027152097', null);
INSERT INTO `mr_host` VALUES ('10789', 'NEW-7-1-2-2', 'NEW-7-1-2-2', '10.10.107.104', 'new0800279FDCB0', null);
INSERT INTO `mr_host` VALUES ('10791', 'NEW-7-3-3-1', 'NEW-7-3-3-1', '10.10.107.121', 'new080027817C8E', null);
INSERT INTO `mr_host` VALUES ('10793', 'NEW-7-3-3-2', 'NEW-7-3-3-2', '10.10.107.122', 'new08002756F0BE', null);
INSERT INTO `mr_host` VALUES ('10794', 'NEW-7-1-3-1', 'NEW-7-1-3-1', '10.10.107.106', 'new080027A39400', null);
INSERT INTO `mr_host` VALUES ('10795', 'NEW-5-4-1-2', 'NEW-5-4-1-2', '10.10.105.125', 'new080027C91EBC', null);
INSERT INTO `mr_host` VALUES ('10796', 'NEW-7-4-2-1', 'NEW-7-4-2-1', '10.10.107.127', 'new08002756A517', null);
INSERT INTO `mr_host` VALUES ('10798', 'NEW-7-2-2-1', 'NEW-7-2-2-1', '10.10.107.112', 'new080027CEF62B', null);
INSERT INTO `mr_host` VALUES ('10799', 'NEW-7-5-2-2', 'NEW-7-5-2-2', '10.10.107.136', 'new080027B834F0', null);
INSERT INTO `mr_host` VALUES ('10800', 'NEW-7-2-2-2', 'NEW-7-2-2-2', '10.10.107.111', 'new080027D062CB', null);
INSERT INTO `mr_host` VALUES ('10817', 'NEW-7-2-1-2', 'NEW-7-2-1-2', '10.10.107.109', 'new080027703081', null);
INSERT INTO `mr_host` VALUES ('10821', 'NEW-7-3-4-1', 'NEW-7-3-4-1', '10.10.107.123', 'new080027344053', null);
INSERT INTO `mr_host` VALUES ('10822', 'NEW-7-1-3-2', 'NEW-7-1-3-2', '10.10.107.105', 'new080027D707CC', null);
INSERT INTO `mr_host` VALUES ('10824', 'NEW-7-2-1-1', 'NEW-7-2-1-1', '10.10.107.110', 'new080027F30C0F', null);
INSERT INTO `mr_host` VALUES ('10827', 'NEW-7-2-3-1', 'NEW-7-2-3-1', '10.10.107.113', 'new08002742C968', null);
INSERT INTO `mr_host` VALUES ('10834', 'NEW-7-3-2-2', 'NEW-7-3-2-2', '10.10.107.119', 'new08002788E52E', null);
INSERT INTO `mr_host` VALUES ('10837', 'NEW-7-3-1-2', 'NEW-7-3-1-2', '10.10.107.117', 'new08002776A472', null);
INSERT INTO `mr_host` VALUES ('10858', 'NEW-7-4-1-1', 'NEW-7-4-1-1', '10.10.107.126', 'new080027B58DCE', null);
INSERT INTO `mr_host` VALUES ('10859', 'NEW-7-2-4-1', 'NEW-7-2-4-1', '10.10.107.116', 'new080027EF4377', null);
INSERT INTO `mr_host` VALUES ('10860', 'NEW-7-5-4-2', 'NEW-7-5-4-2', '10.10.107.139', 'new0800271728E4', null);
INSERT INTO `mr_host` VALUES ('10861', 'NEW-7-5-3-1', 'NEW-7-5-3-1', '10.10.107.137', 'new08002739FBC9', null);
INSERT INTO `mr_host` VALUES ('10862', 'NEW-7-4-4-1', 'NEW-7-4-4-1', '10.10.107.131', 'new080027036BBF', null);
INSERT INTO `mr_host` VALUES ('10863', 'NEW-7-1-1-2', 'NEW-7-1-1-2', '10.10.107.102', 'new0800271FF76C', null);
INSERT INTO `mr_host` VALUES ('10864', 'NEW-7-3-1-1', 'NEW-7-3-1-1', '10.10.107.118', 'new080027FCF7AD', null);
INSERT INTO `mr_host` VALUES ('10865', 'NEW-6-5-2-1', 'NEW-6-5-2-1', '10.10.106.135', 'new080027664D98', null);
INSERT INTO `mr_host` VALUES ('10866', 'NEW-6-5-2-2', 'NEW-6-5-2-2', '10.10.106.136', 'new08002773AB32', null);
INSERT INTO `mr_host` VALUES ('10867', 'NEW-6-5-3-1', 'NEW-6-5-3-1', '10.10.106.137', 'new0800275FE53F', null);
INSERT INTO `mr_host` VALUES ('10868', 'NEW-6-5-3-2', 'NEW-6-5-3-2', '10.10.106.138', 'new0800275F1235', null);
INSERT INTO `mr_host` VALUES ('10869', 'NEW-6-3-2-1', 'NEW-6-3-2-1', '10.10.106.120', 'new080027C7E2E1', null);
INSERT INTO `mr_host` VALUES ('10870', 'NEW-6-5-1-2', 'NEW-6-5-1-2', '10.10.106.134', 'new0800278941C4', null);
INSERT INTO `mr_host` VALUES ('10872', 'NEW-6-3-3-2', 'NEW-6-3-3-2', '10.10.106.121', 'new080027856E62', null);
INSERT INTO `mr_host` VALUES ('10873', 'NEW-6-5-4-1', 'NEW-6-5-4-1', '10.10.106.140', 'new080027A70AAF', null);
INSERT INTO `mr_host` VALUES ('10874', 'NEW-6-5-1-1', 'NEW-6-5-1-1', '10.10.106.133', 'new08002748030F', null);
INSERT INTO `mr_host` VALUES ('10875', 'NEW-6-5-4-2', 'NEW-6-5-4-2', '10.10.106.139', 'new08002779019E', null);
INSERT INTO `mr_host` VALUES ('10876', 'NEW-6-2-3-2', 'NEW-6-2-3-2', '10.10.106.114', 'new080027BCD47D', null);
INSERT INTO `mr_host` VALUES ('10877', 'NEW-6-3-3-1', 'NEW-6-3-3-1', '10.10.106.122', 'new080027A4C5F5', null);
INSERT INTO `mr_host` VALUES ('10878', 'NEW-6-3-2-2', 'NEW-6-3-2-2', '10.10.106.119', 'new08002773E906', null);
INSERT INTO `mr_host` VALUES ('10879', 'NEW-6-2-3-1', 'NEW-6-2-3-1', '10.10.106.113', 'new080027FC0FFC', null);
INSERT INTO `mr_host` VALUES ('10880', 'NEW-6-1-2-2', 'NEW-6-1-2-2', '10.10.106.103', 'new0800273964FB', null);
INSERT INTO `mr_host` VALUES ('10881', 'NEW-6-2-2-1', 'NEW-6-2-2-1', '10.10.106.111', 'new0800273DABB2', null);
INSERT INTO `mr_host` VALUES ('10882', 'NEW-6-3-1-1', 'NEW-6-3-1-1', '10.10.106.117', 'new080027ED3EFB', null);
INSERT INTO `mr_host` VALUES ('10883', 'NEW-6-3-1-2', 'NEW-6-3-1-2', '10.10.106.118', 'new08002700A6BA', null);
INSERT INTO `mr_host` VALUES ('10884', 'NEW-6-3-4-1', 'NEW-6-3-4-1', '10.10.106.123', 'new0800275D7D4B', null);
INSERT INTO `mr_host` VALUES ('10885', 'NEW-6-4-3-1', 'NEW-6-4-3-1', '10.10.106.129', 'new080027F58826', null);
INSERT INTO `mr_host` VALUES ('10886', 'NEW-6-1-3-2', 'NEW-6-1-3-2', '10.10.106.105', 'new080027037DCB', null);
INSERT INTO `mr_host` VALUES ('10887', 'NEW-6-4-3-2', 'NEW-6-4-3-2', '10.10.106.130', 'new080027E2D43B', null);
INSERT INTO `mr_host` VALUES ('10889', 'NEW-6-2-2-2', 'NEW-6-2-2-2', '10.10.106.112', 'new08002720B77C', null);
INSERT INTO `mr_host` VALUES ('10890', 'NEW-6-1-2-1', 'NEW-6-1-2-1', '10.10.106.104', 'new080027C0B8A4', null);
INSERT INTO `mr_host` VALUES ('10891', 'NEW-6-4-4-2', 'NEW-6-4-4-2', '10.10.106.131', 'new08002712E318', null);
INSERT INTO `mr_host` VALUES ('10892', 'NEW-6-3-4-2', 'NEW-6-3-4-2', '10.10.106.124', 'new0800277D075C', null);
INSERT INTO `mr_host` VALUES ('10893', 'NEW-6-4-4-1', 'NEW-6-4-4-1', '10.10.106.132', 'new080027647E08', null);
INSERT INTO `mr_host` VALUES ('10894', 'NEW-6-4-2-1', 'NEW-6-4-2-1', '10.10.106.127', 'new08002715EA79', null);
INSERT INTO `mr_host` VALUES ('10895', 'NEW-6-1-1-1', 'NEW-6-1-1-1', '10.10.106.102', 'new080027ABE7D6', null);
INSERT INTO `mr_host` VALUES ('10896', 'NEW-6-4-1-2', 'NEW-6-4-1-2', '10.10.106.125', 'new080027E151F8', null);
INSERT INTO `mr_host` VALUES ('10897', 'NEW-6-1-3-1', 'NEW-6-1-3-1', '10.10.106.106', 'new080027183037', null);
INSERT INTO `mr_host` VALUES ('10898', 'NEW-6-4-2-2', 'NEW-6-4-2-2', '10.10.106.128', 'new0800278798FC', null);
INSERT INTO `mr_host` VALUES ('10899', 'NEW-6-4-1-1', 'NEW-6-4-1-1', '10.10.106.126', 'new080027D2A4FB', null);
INSERT INTO `mr_host` VALUES ('10901', 'NEW-6-2-1-1', 'NEW-6-2-1-1', '10.10.106.109', 'new08002706D144', null);
INSERT INTO `mr_host` VALUES ('10902', 'NEW-6-2-1-2', 'NEW-6-2-1-2', '10.10.106.110', 'new08002721DEB8', null);
INSERT INTO `mr_host` VALUES ('10903', 'NEW-6-1-4-1', 'NEW-6-1-4-1', '10.10.106.107', 'new080027C7C27C', null);
INSERT INTO `mr_host` VALUES ('10904', 'NEW-6-2-4-2', 'NEW-6-2-4-2', '10.10.106.115', 'new080027A97E41', null);
INSERT INTO `mr_host` VALUES ('10905', 'NEW-6-1-1-2', 'NEW-6-1-1-2', '10.10.106.101', 'new0800277A77ED', null);
INSERT INTO `mr_host` VALUES ('10906', 'NEW-6-1-4-2', 'NEW-6-1-4-2', '10.10.106.108', 'new08002785BD75', null);
INSERT INTO `mr_host` VALUES ('10907', 'NEW-6-2-4-1', 'NEW-6-2-4-1', '10.10.106.116', 'new08002715F837', null);
INSERT INTO `mr_host` VALUES ('10908', 'NEW-1-3-1-1', 'NEW-1-3-1-1', '10.10.101.106', 'new080027A145AC', null);
INSERT INTO `mr_host` VALUES ('10909', 'NEW-1-4-1-1', 'NEW-1-4-1-1', '10.10.101.126', 'new0800276D15E9', null);
INSERT INTO `mr_host` VALUES ('10915', 'NEW-1-5-1-2', 'NEW-1-5-1-2', '10.10.101.133', 'new080027D79189', null);
INSERT INTO `mr_host` VALUES ('10918', 'NEW-4-1-3-1', 'NEW-4-1-3-1', '10.10.104.105', 'new080027B368CD', null);
INSERT INTO `mr_host` VALUES ('10960', 'NEW-3-2-1-1', 'NEW-3-2-1-1', '10.10.103.110', 'new0800272CE050', null);
INSERT INTO `mr_host` VALUES ('10961', 'NEW-2-2-1-2', 'NEW-2-2-1-2', '10.10.102.110', 'new080027B14207', null);
INSERT INTO `mr_host` VALUES ('10962', 'NEW-3-5-2-1', 'NEW-3-5-2-1', '10.10.103.135', 'new080027B53013', null);
INSERT INTO `mr_host` VALUES ('10964', 'NEW-3-1-1-2', 'NEW-3-1-1-2', '10.10.103.102', 'new08002793FA1E', null);
INSERT INTO `mr_host` VALUES ('10966', 'NEW-4-1-3-2', 'NEW-4-1-3-2', '10.10.104.106', 'new080027689F7B', null);
INSERT INTO `mr_host` VALUES ('10968', 'NEW-3-1-3-2', 'NEW-3-1-3-2', '10.10.103.106', 'new080027E69D6D', null);
INSERT INTO `mr_host` VALUES ('10970', 'NEW-3-4-1-1', 'NEW-3-4-1-1', '10.10.103.125', 'new080027C1976C', null);
INSERT INTO `mr_host` VALUES ('10972', 'NEW-3-4-3-1', 'NEW-3-4-3-1', '10.10.103.129', 'new0800278B4BC2', null);
INSERT INTO `mr_host` VALUES ('10974', 'NEW-2-2-2-1', 'NEW-2-2-2-1', '10.10.102.111', 'new080027098A92', null);
INSERT INTO `mr_host` VALUES ('10976', 'NEW-4-2-1-1', 'NEW-4-2-1-1', '10.10.104.109', 'new080027F02D44', null);
INSERT INTO `mr_host` VALUES ('10978', 'NEW-3-2-2-1', 'NEW-3-2-2-1', '10.10.103.111', 'new080027D330ED', null);
INSERT INTO `mr_host` VALUES ('10980', 'NEW-3-3-1-1', 'NEW-3-3-1-1', '10.10.103.118', 'new080027478C01', null);
INSERT INTO `mr_host` VALUES ('10982', 'NEW-3-3-2-2', 'NEW-3-3-2-2', '10.10.103.120', 'new0800272A1036', null);
INSERT INTO `mr_host` VALUES ('10984', 'NEW-3-4-4-1', 'NEW-3-4-4-1', '10.10.103.132', 'new080027B82884', null);
INSERT INTO `mr_host` VALUES ('10986', 'NEW-3-1-2-1', 'NEW-3-1-2-1', '10.10.103.103', 'new0800276F5E5F', null);
INSERT INTO `mr_host` VALUES ('10988', 'NEW-3-3-3-1', 'NEW-3-3-3-1', '10.10.103.122', 'new0800272CEC6C', null);
INSERT INTO `mr_host` VALUES ('10990', 'NEW-3-5-1-1', 'NEW-3-5-1-1', '10.10.103.133', 'new080027C66FF1', null);
INSERT INTO `mr_host` VALUES ('10992', 'NEW-3-1-3-1', 'NEW-3-1-3-1', '10.10.103.105', 'new08002713723E', null);
INSERT INTO `mr_host` VALUES ('10994', 'NEW-3-2-3-1', 'NEW-3-2-3-1', '10.10.103.114', 'new080027E2FE17', null);
INSERT INTO `mr_host` VALUES ('10996', 'NEW-3-1-1-1', 'NEW-3-1-1-1', '10.10.103.101', 'new08002764DF81', null);
INSERT INTO `mr_host` VALUES ('10998', 'NEW-2-4-4-2', 'NEW-2-4-4-2', '10.10.102.132', 'new080027873965', null);
INSERT INTO `mr_host` VALUES ('11000', 'NEW-3-3-4-2', 'NEW-3-3-4-2', '10.10.103.123', 'new080027C33408', null);
INSERT INTO `mr_host` VALUES ('11002', 'NEW-4-4-3-1', 'NEW-4-4-3-1', '10.10.104.130', 'new0800279DDDA6', null);
INSERT INTO `mr_host` VALUES ('11004', 'NEW-2-4-3-2', 'NEW-2-4-3-2', '10.10.102.130', 'new080027C74050', null);
INSERT INTO `mr_host` VALUES ('11006', 'NEW-4-5-3-2', 'NEW-4-5-3-2', '10.10.104.137', 'new08002715F727', null);
INSERT INTO `mr_host` VALUES ('11008', 'NEW-4-3-1-1', 'NEW-4-3-1-1', '10.10.104.117', 'new0800270B05DF', null);
INSERT INTO `mr_host` VALUES ('11010', 'NEW-2-5-4-1', 'NEW-2-5-4-1', '10.10.102.139', 'new0800275C9EA9', null);
INSERT INTO `mr_host` VALUES ('11012', 'NEW-2-3-4-2', 'NEW-2-3-4-2', '10.10.102.124', 'new080027E96AEF', null);
INSERT INTO `mr_host` VALUES ('11014', 'NEW-3-1-4-2', 'NEW-3-1-4-2', '10.10.103.108', 'new080027D610D8', null);
INSERT INTO `mr_host` VALUES ('11016', 'NEW-4-1-1-2', 'NEW-4-1-1-2', '10.10.104.101', 'new080027EF0ED5', null);
INSERT INTO `mr_host` VALUES ('11018', 'NEW-3-4-1-2', 'NEW-3-4-1-2', '10.10.103.126', 'new080027C0EE66', null);
INSERT INTO `mr_host` VALUES ('11020', 'NEW-4-5-1-2', 'NEW-4-5-1-2', '10.10.104.134', 'new080027AA6849', null);
INSERT INTO `mr_host` VALUES ('11022', 'NEW-3-4-2-2', 'NEW-3-4-2-2', '10.10.103.128', 'new0800273D97A4', null);
INSERT INTO `mr_host` VALUES ('11024', 'NEW-3-2-3-2', 'NEW-3-2-3-2', '10.10.103.113', 'new08002796E83E', null);
INSERT INTO `mr_host` VALUES ('11026', 'NEW-4-5-1-1', 'NEW-4-5-1-1', '10.10.104.133', 'new08002701AABF', null);
INSERT INTO `mr_host` VALUES ('11028', 'NEW-3-5-4-1', 'NEW-3-5-4-1', '10.10.103.140', 'new08002766D5ED', null);
INSERT INTO `mr_host` VALUES ('11030', 'NEW-3-2-2-2', 'NEW-3-2-2-2', '10.10.103.112', 'new0800279CA2D3', null);
INSERT INTO `mr_host` VALUES ('11032', 'NEW-4-4-2-2', 'NEW-4-4-2-2', '10.10.104.127', 'new080027590EAC', null);
INSERT INTO `mr_host` VALUES ('11034', 'NEW-3-3-1-2', 'NEW-3-3-1-2', '10.10.103.117', 'new080027538B56', null);
INSERT INTO `mr_host` VALUES ('11036', 'NEW-1-5-4-1', 'NEW-1-5-4-1', '10.10.101.140', 'new08002734940D', null);
INSERT INTO `mr_host` VALUES ('11038', 'NEW-4-4-1-2', 'NEW-4-4-1-2', '10.10.104.125', 'new08002784BB85', null);
INSERT INTO `mr_host` VALUES ('11040', 'NEW-4-5-2-1', 'NEW-4-5-2-1', '10.10.104.135', 'new080027C076FA', null);
INSERT INTO `mr_host` VALUES ('11042', 'NEW-4-1-2-2', 'NEW-4-1-2-2', '10.10.104.104', 'new080027DFC969', null);
INSERT INTO `mr_host` VALUES ('11044', 'NEW-4-5-4-2', 'NEW-4-5-4-2', '10.10.104.140', 'new080027B4AAAC', null);
INSERT INTO `mr_host` VALUES ('11046', 'NEW-4-2-2-2', 'NEW-4-2-2-2', '10.10.104.112', 'new0800273209BE', null);
INSERT INTO `mr_host` VALUES ('11048', 'NEW-4-5-3-1', 'NEW-4-5-3-1', '10.10.104.138', 'new080027B9C8C2', null);
INSERT INTO `mr_host` VALUES ('11050', 'NEW-3-5-1-2', 'NEW-3-5-1-2', '10.10.103.134', 'new08002765FEF8', null);
INSERT INTO `mr_host` VALUES ('11052', 'NEW-3-5-3-1', 'NEW-3-5-3-1', '10.10.103.137', 'new080027F91C29', null);
INSERT INTO `mr_host` VALUES ('11054', 'NEW-4-5-4-1', 'NEW-4-5-4-1', '10.10.104.139', 'new0800275247B1', null);
INSERT INTO `mr_host` VALUES ('11056', 'NEW-4-1-4-2', 'NEW-4-1-4-2', '10.10.104.108', 'new080027819354', null);
INSERT INTO `mr_host` VALUES ('11058', 'NEW-1-4-2-1', 'NEW-1-4-2-1', '10.10.101.127', 'new0800270247DC', null);
INSERT INTO `mr_host` VALUES ('11060', 'NEW-2-3-1-1', 'NEW-2-3-1-1', '10.10.102.118', 'new08002772925E', null);
INSERT INTO `mr_host` VALUES ('11062', 'NEW-2-1-2-2', 'NEW-2-1-2-2', '10.10.102.104', 'new080027B15CC3', null);
INSERT INTO `mr_host` VALUES ('11064', 'NEW-2-5-2-1', 'NEW-2-5-2-1', '10.10.102.135', 'new080027C272F3', null);
INSERT INTO `mr_host` VALUES ('11066', 'NEW-4-2-3-1', 'NEW-4-2-3-1', '10.10.104.114', 'new080027D48D22', null);
INSERT INTO `mr_host` VALUES ('11068', 'NEW-4-3-4-1', 'NEW-4-3-4-1', '10.10.104.124', 'new080027965E33', null);
INSERT INTO `mr_host` VALUES ('11070', 'NEW-3-2-4-2', 'NEW-3-2-4-2', '10.10.103.115', 'new0800272C6822', null);
INSERT INTO `mr_host` VALUES ('11072', 'NEW-2-1-4-1', 'NEW-2-1-4-1', '10.10.102.108', 'new080027E27959', null);
INSERT INTO `mr_host` VALUES ('11074', 'NEW-2-1-1-2', 'NEW-2-1-1-2', '10.10.102.102', 'new0800273327F8', null);
INSERT INTO `mr_host` VALUES ('11076', 'NEW-4-3-2-2', 'NEW-4-3-2-2', '10.10.104.120', 'new080027BF894A', null);
INSERT INTO `mr_host` VALUES ('11078', 'NEW-2-2-4-1', 'NEW-2-2-4-1', '10.10.102.115', 'new080027032B94', null);
INSERT INTO `mr_host` VALUES ('11080', 'NEW-1-2-1-2', 'NEW-1-2-1-2', '10.10.101.103', 'new080027799F00', null);
INSERT INTO `mr_host` VALUES ('11082', 'NEW-4-4-3-2', 'NEW-4-4-3-2', '10.10.104.129', 'new0800274E22A3', null);
INSERT INTO `mr_host` VALUES ('11084', 'NEW-4-4-1-1', 'NEW-4-4-1-1', '10.10.104.126', 'new080027E888A4', null);
INSERT INTO `mr_host` VALUES ('11086', 'NEW-3-2-1-2', 'NEW-3-2-1-2', '10.10.103.109', 'new080027A6BE9C', null);
INSERT INTO `mr_host` VALUES ('11088', 'NEW-3-4-2-1', 'NEW-3-4-2-1', '10.10.103.127', 'new080027E44860', null);
INSERT INTO `mr_host` VALUES ('11090', 'NEW-4-2-4-1', 'NEW-4-2-4-1', '10.10.104.116', 'new0800275431FC', null);
INSERT INTO `mr_host` VALUES ('11092', 'NEW-3-3-4-1', 'NEW-3-3-4-1', '10.10.103.124', 'new0800275850EC', null);
INSERT INTO `mr_host` VALUES ('11094', 'NEW-2-3-3-2', 'NEW-2-3-3-2', '10.10.102.121', 'new080027369BC5', null);
INSERT INTO `mr_host` VALUES ('11096', 'NEW-2-1-2-1', 'NEW-2-1-2-1', '10.10.102.103', 'new08002736C32F', null);
INSERT INTO `mr_host` VALUES ('11098', 'NEW-4-2-1-2', 'NEW-4-2-1-2', '10.10.104.110', 'new0800270C6BFC', null);
INSERT INTO `mr_host` VALUES ('11100', 'NEW-4-3-3-2', 'NEW-4-3-3-2', '10.10.104.122', 'new0800275F07FC', null);
INSERT INTO `mr_host` VALUES ('11102', 'NEW-3-3-3-2', 'NEW-3-3-3-2', '10.10.103.121', 'new0800274E0986', null);
INSERT INTO `mr_host` VALUES ('11104', 'NEW-2-3-2-1', 'NEW-2-3-2-1', '10.10.102.119', 'new08002783192A', null);
INSERT INTO `mr_host` VALUES ('11106', 'NEW-4-4-4-1', 'NEW-4-4-4-1', '10.10.104.132', 'new080027D3BBCB', null);
INSERT INTO `mr_host` VALUES ('11108', 'NEW-3-4-4-2', 'NEW-3-4-4-2', '10.10.103.131', 'new080027206C15', null);
INSERT INTO `mr_host` VALUES ('11110', 'NEW-2-5-3-1', 'NEW-2-5-3-1', '10.10.102.138', 'new0800278C340B', null);
INSERT INTO `mr_host` VALUES ('11112', 'NEW-2-4-1-2', 'NEW-2-4-1-2', '10.10.102.126', 'new080027663D1E', null);
INSERT INTO `mr_host` VALUES ('11114', 'NEW-4-1-2-1', 'NEW-4-1-2-1', '10.10.104.103', 'new08002792F405', null);
INSERT INTO `mr_host` VALUES ('11116', 'NEW-2-3-2-2', 'NEW-2-3-2-2', '10.10.102.120', 'new08002757BB77', null);
INSERT INTO `mr_host` VALUES ('11118', 'NEW-2-3-3-1', 'NEW-2-3-3-1', '10.10.102.122', 'new080027F81528', null);
INSERT INTO `mr_host` VALUES ('11120', 'NEW-2-2-3-1', 'NEW-2-2-3-1', '10.10.102.114', 'new080027ACF39F', null);
INSERT INTO `mr_host` VALUES ('11122', 'NEW-2-5-1-2', 'NEW-2-5-1-2', '10.10.102.134', 'new0800272EB608', null);
INSERT INTO `mr_host` VALUES ('11124', 'NEW-2-1-1-1', 'NEW-2-1-1-1', '10.10.102.101', 'new0800270C8919', null);
INSERT INTO `mr_host` VALUES ('11126', 'NEW-2-4-4-1', 'NEW-2-4-4-1', '10.10.102.131', 'new080027AD5721', null);
INSERT INTO `mr_host` VALUES ('11128', 'NEW-4-2-4-2', 'NEW-4-2-4-2', '10.10.104.115', 'new080027D9BCE6', null);
INSERT INTO `mr_host` VALUES ('11130', 'NEW-2-2-4-2', 'NEW-2-2-4-2', '10.10.102.116', 'new0800279C86BA', null);
INSERT INTO `mr_host` VALUES ('11132', 'NEW-2-5-4-2', 'NEW-2-5-4-2', '10.10.102.140', 'new080027965E3B', null);
INSERT INTO `mr_host` VALUES ('11134', 'NEW-3-1-2-2', 'NEW-3-1-2-2', '10.10.103.104', 'new0800274A78B6', null);
INSERT INTO `mr_host` VALUES ('11136', 'NEW-2-2-1-1', 'NEW-2-2-1-1', '10.10.102.109', 'new08002767A8A4', null);
INSERT INTO `mr_host` VALUES ('11138', 'NEW-4-4-4-2', 'NEW-4-4-4-2', '10.10.104.131', 'new0800279C65EC', null);
INSERT INTO `mr_host` VALUES ('11140', 'NEW-2-5-1-1', 'NEW-2-5-1-1', '10.10.102.133', 'new080027BB4DAF', null);
INSERT INTO `mr_host` VALUES ('11142', 'NEW-4-1-4-1', 'NEW-4-1-4-1', '10.10.104.107', 'new08002770A3F7', null);
INSERT INTO `mr_host` VALUES ('11144', 'NEW-1-5-4-2', 'NEW-1-5-4-2', '10.10.101.139', 'new080027A6F335', null);
INSERT INTO `mr_host` VALUES ('11146', 'NEW-1-5-2-2', 'NEW-1-5-2-2', '10.10.101.136', 'new080027F9F307', null);
INSERT INTO `mr_host` VALUES ('11148', 'NEW-1-1-4-1', 'NEW-1-1-4-1', '10.10.101.112', 'new08002771BB7C', null);
INSERT INTO `mr_host` VALUES ('11150', 'NEW-2-3-1-2', 'NEW-2-3-1-2', '10.10.102.117', 'new0800276663A1', null);
INSERT INTO `mr_host` VALUES ('11152', 'NEW-2-2-2-2', 'NEW-2-2-2-2', '10.10.102.112', 'new080027EF749F', null);
INSERT INTO `mr_host` VALUES ('11154', 'NEW-2-4-2-1', 'NEW-2-4-2-1', '10.10.102.128', 'new080027B86A13', null);
INSERT INTO `mr_host` VALUES ('11156', 'NEW-1-5-2-1', 'NEW-1-5-2-1', '10.10.101.135', 'new080027B15A5B', null);
INSERT INTO `mr_host` VALUES ('11158', 'NEW-2-5-2-2', 'NEW-2-5-2-2', '10.10.102.136', 'new08002780B8F9', null);
INSERT INTO `mr_host` VALUES ('11160', 'NEW-1-4-1-2', 'NEW-1-4-1-2', '10.10.101.125', 'new08002714CF73', null);
INSERT INTO `mr_host` VALUES ('11162', 'NEW-1-3-4-2', 'NEW-1-3-4-2', '10.10.101.123', 'new08002767562E', null);
INSERT INTO `mr_host` VALUES ('11164', 'NEW-4-3-1-2', 'NEW-4-3-1-2', '10.10.104.118', 'new0800279725CD', null);
INSERT INTO `mr_host` VALUES ('11166', 'NEW-1-4-4-2', 'NEW-1-4-4-2', '10.10.101.131', 'new08002793CC8A', null);
INSERT INTO `mr_host` VALUES ('11168', 'NEW-1-2-2-2', 'NEW-1-2-2-2', '10.10.101.114', 'new08002763D2D7', null);
INSERT INTO `mr_host` VALUES ('11170', 'NEW-3-2-4-1', 'NEW-3-2-4-1', '10.10.103.116', 'new08002788FA42', null);
INSERT INTO `mr_host` VALUES ('11172', 'NEW-1-2-1-1', 'NEW-1-2-1-1', '10.10.101.104', 'new080027094DD8', null);
INSERT INTO `mr_host` VALUES ('11174', 'NEW-2-4-1-1', 'NEW-2-4-1-1', '10.10.102.125', 'new0800270C7E44', null);
INSERT INTO `mr_host` VALUES ('11176', 'NEW-1-2-4-1', 'NEW-1-2-4-1', '10.10.101.118', 'new080027F00919', null);
INSERT INTO `mr_host` VALUES ('11178', 'NEW-1-1-4-2', 'NEW-1-1-4-2', '10.10.101.111', 'new080027A3C893', null);
INSERT INTO `mr_host` VALUES ('11180', 'NEW-4-3-3-1', 'NEW-4-3-3-1', '10.10.104.121', 'new080027DA4553', null);
INSERT INTO `mr_host` VALUES ('11182', 'NEW-1-2-3-2', 'NEW-1-2-3-2', '10.10.101.116', 'new080027761C7F', null);
INSERT INTO `mr_host` VALUES ('11184', 'NEW-1-3-3-2', 'NEW-1-3-3-2', '10.10.101.121', 'new0800273D88D6', null);
INSERT INTO `mr_host` VALUES ('11186', 'NEW-1-4-3-2', 'NEW-1-4-3-2', '10.10.101.129', 'new0800277AD46F', null);
INSERT INTO `mr_host` VALUES ('11188', 'NEW-1-1-3-1', 'NEW-1-1-3-1', '10.10.101.109', 'new080027B150F7', null);
INSERT INTO `mr_host` VALUES ('11190', 'NEW-1-2-3-1', 'NEW-1-2-3-1', '10.10.101.115', 'new080027356819', null);
INSERT INTO `mr_host` VALUES ('11192', 'NEW-1-5-3-2', 'NEW-1-5-3-2', '10.10.101.137', 'new080027697AAB', null);
INSERT INTO `mr_host` VALUES ('11194', 'NEW-3-5-3-2', 'NEW-3-5-3-2', '10.10.103.138', 'new0800273B51A0', null);
INSERT INTO `mr_host` VALUES ('11196', 'NEW-1-2-4-2', 'NEW-1-2-4-2', '10.10.101.117', 'new0800276EE7BE', null);
INSERT INTO `mr_host` VALUES ('11198', 'NEW-1-4-3-1', 'NEW-1-4-3-1', '10.10.101.130', 'new0800274C64EE', null);
INSERT INTO `mr_host` VALUES ('11200', 'NEW-1-3-1-2', 'NEW-1-3-1-2', '10.10.101.105', 'new080027237D6A', null);
INSERT INTO `mr_host` VALUES ('11202', 'NEW-1-1-3-2', 'NEW-1-1-3-2', '10.10.101.110', 'new080027B210E2', null);
INSERT INTO `mr_host` VALUES ('11204', 'NEW-1-3-4-1', 'NEW-1-3-4-1', '10.10.101.124', 'new080027B343EF', null);
INSERT INTO `mr_host` VALUES ('11206', 'NEW-2-1-3-2', 'NEW-2-1-3-2', '10.10.102.105', 'new08002753AF1A', null);
INSERT INTO `mr_host` VALUES ('11208', 'NEW-1-1-2-1', 'NEW-1-1-2-1', '10.10.101.107', 'new080027707B7D', null);
INSERT INTO `mr_host` VALUES ('11210', 'NEW-1-3-3-1', 'NEW-1-3-3-1', '10.10.101.122', 'new080027CFFB44', null);
INSERT INTO `mr_host` VALUES ('11212', 'NEW-1-5-1-1', 'NEW-1-5-1-1', '10.10.101.134', 'new08002702F3BC', null);
INSERT INTO `mr_host` VALUES ('11214', 'NEW-1-3-2-2', 'NEW-1-3-2-2', '10.10.101.120', 'new080027BFB11F', null);
INSERT INTO `mr_host` VALUES ('11216', 'NEW-1-2-2-1', 'NEW-1-2-2-1', '10.10.101.113', 'new080027CA866F', null);
INSERT INTO `mr_host` VALUES ('11218', 'NEW-3-4-3-2', 'NEW-3-4-3-2', '10.10.103.130', 'new080027006B3F', null);
INSERT INTO `mr_host` VALUES ('11220', 'NEW-1-3-2-1', 'NEW-1-3-2-1', '10.10.101.119', 'new08002714A247', null);
INSERT INTO `mr_host` VALUES ('11222', 'NEW-1-4-2-2', 'NEW-1-4-2-2', '10.10.101.128', 'new0800277ED0C7', null);
INSERT INTO `mr_host` VALUES ('11224', 'NEW-1-5-3-1', 'NEW-1-5-3-1', '10.10.101.138', 'new080027CA8DD1', null);
INSERT INTO `mr_host` VALUES ('11226', 'NEW-3-5-2-2', 'NEW-3-5-2-2', '10.10.103.136', 'new080027DF4D02', null);
INSERT INTO `mr_host` VALUES ('11228', 'NEW-1-1-1-2', 'NEW-1-1-1-2', '10.10.101.101', 'new080027CD3D31', null);
INSERT INTO `mr_host` VALUES ('11230', 'NEW-1-1-1-1', 'NEW-1-1-1-1', '10.10.101.102', 'new08002734EF5F', null);
INSERT INTO `mr_host` VALUES ('11232', 'NEW-4-2-3-2', 'NEW-4-2-3-2', '10.10.104.113', 'new080027BCCF7B', null);
INSERT INTO `mr_host` VALUES ('11234', 'NEW-4-2-2-1', 'NEW-4-2-2-1', '10.10.104.111', 'new080027A7F668', null);
INSERT INTO `mr_host` VALUES ('11235', 'NEW-3-3-2-1', 'NEW-3-3-2-1', '10.10.103.119', 'new080027557788', null);
INSERT INTO `mr_host` VALUES ('11236', 'NEW-2-4-3-1', 'NEW-2-4-3-1', '10.10.102.129', 'new08002761178C', null);
INSERT INTO `mr_host` VALUES ('11237', 'NEW-4-4-2-1', 'NEW-4-4-2-1', '10.10.104.128', 'new080027823FE1', null);
INSERT INTO `mr_host` VALUES ('11238', 'NEW-3-5-4-2', 'NEW-3-5-4-2', '10.10.103.139', 'new080027AF1F2C', null);
INSERT INTO `mr_host` VALUES ('11239', 'NEW-3-1-4-1', 'NEW-3-1-4-1', '10.10.103.107', 'new08002731E083', null);
INSERT INTO `mr_host` VALUES ('11240', 'NEW-1-1-2-2', 'NEW-1-1-2-2', '10.10.101.108', 'new08002794627D', null);
INSERT INTO `mr_host` VALUES ('11242', 'NEW-4-5-2-2', 'NEW-4-5-2-2', '10.10.104.136', 'new080027E655DB', null);
INSERT INTO `mr_host` VALUES ('11243', 'NEW-2-5-3-2', 'NEW-2-5-3-2', '10.10.102.137', 'new080027FA2E8C', null);
INSERT INTO `mr_host` VALUES ('11245', 'NEW-2-2-3-2', 'NEW-2-2-3-2', '10.10.102.113', 'new080027C7FC17', null);
INSERT INTO `mr_host` VALUES ('11246', 'NEW-2-4-2-2', 'NEW-2-4-2-2', '10.10.102.127', 'new080027FF8AD0', null);
INSERT INTO `mr_host` VALUES ('11247', 'NEW-4-3-4-2', 'NEW-4-3-4-2', '10.10.104.123', 'new08002749D32D', null);
INSERT INTO `mr_host` VALUES ('11248', 'NEW-2-1-3-1', 'NEW-2-1-3-1', '10.10.102.106', 'new08002759712F', null);
INSERT INTO `mr_host` VALUES ('11249', 'NEW-2-3-4-1', 'NEW-2-3-4-1', '10.10.102.123', 'new0800271142DC', null);
INSERT INTO `mr_host` VALUES ('11250', 'NEW-1-4-4-1', 'NEW-1-4-4-1', '10.10.101.132', 'new08002709048A', null);
INSERT INTO `mr_host` VALUES ('11251', 'NEW-2-1-4-2', 'NEW-2-1-4-2', '10.10.102.107', 'new080027C8061F', null);
INSERT INTO `mr_host` VALUES ('11252', 'NEW-4-1-1-1', 'NEW-4-1-1-1', '10.10.104.102', 'new080027225131', null);
INSERT INTO `mr_host` VALUES ('11253', 'NEW-4-3-2-1', 'NEW-4-3-2-1', '10.10.104.119', 'new0800273314F8', null);
INSERT INTO `mr_host` VALUES ('11254', 'SERVER-NEW', 'SERVER-NEW', '10.10.250.251', 'Server178', null);
INSERT INTO `mr_host` VALUES ('11255', 'NEW-8-2-2-1', 'NEW-8-2-2-1', '10.10.108.112', 'new0800273C3481', null);
INSERT INTO `mr_host` VALUES ('11256', 'NEW-8-2-3-2', 'NEW-8-2-3-2', '10.10.108.113', 'new08002762E240', null);
INSERT INTO `mr_host` VALUES ('11258', 'NEW-8-5-3-2', 'NEW-8-5-3-2', '10.10.108.138', 'new0800274CDE22', null);
INSERT INTO `mr_host` VALUES ('11259', 'NEW-8-2-2-2', 'NEW-8-2-2-2', '10.10.108.111', 'new08002793058D', null);
INSERT INTO `mr_host` VALUES ('11260', 'NEW-8-1-3-2', 'NEW-8-1-3-2', '10.10.108.106', 'new080027B8257A', null);
INSERT INTO `mr_host` VALUES ('11261', 'NEW-8-3-3-1', 'NEW-8-3-3-1', '10.10.108.122', 'new080027E44063', null);
INSERT INTO `mr_host` VALUES ('11262', 'NEW-8-4-4-1', 'NEW-8-4-4-1', '10.10.108.132', 'new08002756BF1C', null);
INSERT INTO `mr_host` VALUES ('11263', 'NEW-8-5-3-1', 'NEW-8-5-3-1', '10.10.108.137', 'new0800272AE70E', null);
INSERT INTO `mr_host` VALUES ('11265', 'NEW-8-5-2-1', 'NEW-8-5-2-1', '10.10.108.135', 'new080027A58B4C', null);
INSERT INTO `mr_host` VALUES ('11266', 'NEW-8-4-3-2', 'NEW-8-4-3-2', '10.10.108.129', 'new080027A24AEB', null);
INSERT INTO `mr_host` VALUES ('11267', 'NEW-8-5-2-2', 'NEW-8-5-2-2', '10.10.108.136', 'new0800277490AF', null);
INSERT INTO `mr_host` VALUES ('11268', 'NEW-8-1-4-2', 'NEW-8-1-4-2', '10.10.108.107', 'new080027F578F3', null);
INSERT INTO `mr_host` VALUES ('11269', 'NEW-8-3-4-2', 'NEW-8-3-4-2', '10.10.108.123', 'new080027CA0124', null);
INSERT INTO `mr_host` VALUES ('11270', 'NEW-8-1-4-1', 'NEW-8-1-4-1', '10.10.108.108', 'new08002789A314', null);
INSERT INTO `mr_host` VALUES ('11271', 'NEW-8-3-4-1', 'NEW-8-3-4-1', '10.10.108.124', 'new080027BB38EA', null);
INSERT INTO `mr_host` VALUES ('11272', 'NEW-8-4-4-2', 'NEW-8-4-4-2', '10.10.108.131', 'new080027B93F8E', null);
INSERT INTO `mr_host` VALUES ('11273', 'NEW-8-2-1-2', 'NEW-8-2-1-2', '10.10.108.109', 'new08002749F0E1', null);
INSERT INTO `mr_host` VALUES ('11274', 'NEW-8-2-3-1', 'NEW-8-2-3-1', '10.10.108.114', 'new080027B4E010', null);
INSERT INTO `mr_host` VALUES ('11275', 'NEW-8-4-3-1', 'NEW-8-4-3-1', '10.10.108.130', 'new0800272F414E', null);
INSERT INTO `mr_host` VALUES ('11276', 'NEW-8-3-3-2', 'NEW-8-3-3-2', '10.10.108.121', 'new080027F0FCA0', null);
INSERT INTO `mr_host` VALUES ('11278', 'NEW-8-1-3-1', 'NEW-8-1-3-1', '10.10.108.105', 'new080027F19E03', null);
INSERT INTO `mr_host` VALUES ('11279', 'NEW-8-4-2-2', 'NEW-8-4-2-2', '10.10.108.128', 'new080027F52531', null);
INSERT INTO `mr_host` VALUES ('11280', 'NEW-8-2-1-1', 'NEW-8-2-1-1', '10.10.108.110', 'new080027C14A65', null);
INSERT INTO `mr_host` VALUES ('11281', 'NEW-8-5-1-1', 'NEW-8-5-1-1', '10.10.108.133', 'new080027A40240', null);
INSERT INTO `mr_host` VALUES ('11282', 'NEW-8-5-4-2', 'NEW-8-5-4-2', '10.10.108.140', 'new080027F397AE', null);
INSERT INTO `mr_host` VALUES ('11283', 'NEW-8-5-4-1', 'NEW-8-5-4-1', '10.10.108.139', 'new0800272EF16C', null);
INSERT INTO `mr_host` VALUES ('11284', 'NEW-8-5-1-2', 'NEW-8-5-1-2', '10.10.108.134', 'new0800277989D2', null);
INSERT INTO `mr_host` VALUES ('11285', 'NEW-8-4-1-1', 'NEW-8-4-1-1', '10.10.108.126', 'new0800271D6A4B', null);
INSERT INTO `mr_host` VALUES ('11286', 'NEW-8-1-1-2', 'NEW-8-1-1-2', '10.10.108.102', 'new0800270A5890', null);
INSERT INTO `mr_host` VALUES ('11288', 'NEW-8-1-2-2', 'NEW-8-1-2-2', '10.10.108.103', 'new080027609149', null);
INSERT INTO `mr_host` VALUES ('11289', 'NEW-8-1-2-1', 'NEW-8-1-2-1', '10.10.108.104', 'new0800274023ED', null);
INSERT INTO `mr_host` VALUES ('11291', 'NEW-8-1-1-1', 'NEW-8-1-1-1', '10.10.108.101', 'new0800270F7687', null);
INSERT INTO `mr_host` VALUES ('11293', 'NEW-8-3-1-2', 'NEW-8-3-1-2', '10.10.108.117', 'new080027A89C44', null);
INSERT INTO `mr_host` VALUES ('11295', 'NEW-8-3-2-2', 'NEW-8-3-2-2', '10.10.108.119', 'new0800273AA955', null);
INSERT INTO `mr_host` VALUES ('11296', 'NEW-8-3-2-1', 'NEW-8-3-2-1', '10.10.108.120', 'new0800277B6709', null);
INSERT INTO `mr_host` VALUES ('11297', 'NEW-8-3-1-1', 'NEW-8-3-1-1', '10.10.108.118', 'new080027A490BE', null);
INSERT INTO `mr_host` VALUES ('11298', 'NEW-8-2-4-1', 'NEW-8-2-4-1', '10.10.108.116', 'new0800273070B2', null);
INSERT INTO `mr_host` VALUES ('11300', 'NEW-8-4-1-2', 'NEW-8-4-1-2', '10.10.108.125', 'new080027C07FB0', null);
INSERT INTO `mr_host` VALUES ('11301', 'NEW-8-4-2-1', 'NEW-8-4-2-1', '10.10.108.127', 'new0800275B5B3D', null);
INSERT INTO `mr_host` VALUES ('11302', 'NEW-8-2-4-2', 'NEW-8-2-4-2', '10.10.108.115', 'new0800279CAC4D', null);
INSERT INTO `mr_host` VALUES ('11329', 'OLD-3-4-2-1', 'OLD-3-4-2-1', '10.10.203.127', 'old080027F7ED5D', null);
INSERT INTO `mr_host` VALUES ('11333', 'OLD-4-5-3-2', 'OLD-4-5-3-2', '10.10.204.137', 'old080027871707', null);
INSERT INTO `mr_host` VALUES ('11337', 'OLD-4-1-4-2', 'OLD-4-1-4-2', '10.10.204.108', 'old08002720AEA3', null);
INSERT INTO `mr_host` VALUES ('11339', 'OLD-4-3-3-1', 'OLD-4-3-3-1', '10.10.204.121', 'old080027577EF5', null);
INSERT INTO `mr_host` VALUES ('11341', 'OLD-4-3-3-2', 'OLD-4-3-3-2', '10.10.204.122', 'old080027515116', null);
INSERT INTO `mr_host` VALUES ('11344', 'OLD-4-2-3-2', 'OLD-4-2-3-2', '10.10.204.113', 'old080027370BF3', null);
INSERT INTO `mr_host` VALUES ('11350', 'OLD-4-1-4-1', 'OLD-4-1-4-1', '10.10.204.107', 'old0800278E33D5', null);
INSERT INTO `mr_host` VALUES ('11360', 'OLD-4-2-4-2', 'OLD-4-2-4-2', '10.10.204.116', 'old080027BD834A', null);
INSERT INTO `mr_host` VALUES ('11368', 'OLD-4-2-3-1', 'OLD-4-2-3-1', '10.10.204.114', 'old0800272F4F72', null);
INSERT INTO `mr_host` VALUES ('11370', 'OLD-4-1-3-2', 'OLD-4-1-3-2', '10.10.204.106', 'old080027C162DF', null);
INSERT INTO `mr_host` VALUES ('11372', 'OLD-4-2-4-1', 'OLD-4-2-4-1', '10.10.204.115', 'old08002759F481', null);
INSERT INTO `mr_host` VALUES ('11375', 'OLD-4-5-4-2', 'OLD-4-5-4-2', '10.10.204.139', 'old08002724CB79', null);
INSERT INTO `mr_host` VALUES ('11377', 'OLD-4-5-4-1', 'OLD-4-5-4-1', '10.10.204.140', 'old08002723B8A6', null);
INSERT INTO `mr_host` VALUES ('11382', 'OLD-1-1-3-2', 'OLD-1-1-3-2', '10.10.201.110', 'old080027354919', null);
INSERT INTO `mr_host` VALUES ('11385', 'OLD-4-3-4-2', 'OLD-4-3-4-2', '10.10.204.124', 'old0800272E6F47', null);
INSERT INTO `mr_host` VALUES ('11389', 'OLD-3-2-3-1', 'OLD-3-2-3-1', '10.10.203.113', 'old0800278D6F21', null);
INSERT INTO `mr_host` VALUES ('11390', 'OLD-2-3-3-1', 'OLD-2-3-3-1', '10.10.202.121', 'old08002766A5BF', null);
INSERT INTO `mr_host` VALUES ('11391', 'OLD-4-4-3-2', 'OLD-4-4-3-2', '10.10.204.129', 'old080027D198D2', null);
INSERT INTO `mr_host` VALUES ('11393', 'OLD-3-4-3-1', 'OLD-3-4-3-1', '10.10.203.130', 'old0800270C60EE', null);
INSERT INTO `mr_host` VALUES ('11395', 'OLD-3-1-2-2', 'OLD-3-1-2-2', '10.10.203.104', 'old08002742C1C2', null);
INSERT INTO `mr_host` VALUES ('11396', 'OLD-2-1-3-1', 'OLD-2-1-3-1', '10.10.202.105', 'old0800274353F3', null);
INSERT INTO `mr_host` VALUES ('11398', 'OLD-2-2-2-2', 'OLD-2-2-2-2', '10.10.202.111', 'old080027856310', null);
INSERT INTO `mr_host` VALUES ('11399', 'OLD-2-5-3-2', 'OLD-2-5-3-2', '10.10.202.137', 'old080027BBDB4F', null);
INSERT INTO `mr_host` VALUES ('11401', 'OLD-2-5-3-1', 'OLD-2-5-3-1', '10.10.202.138', 'old0800271DB2C3', null);
INSERT INTO `mr_host` VALUES ('11403', 'OLD-1-2-3-2', 'OLD-1-2-3-2', '10.10.201.115', 'old08002755483D', null);
INSERT INTO `mr_host` VALUES ('11405', 'OLD-1-2-3-1', 'OLD-1-2-3-1', '10.10.201.116', 'old08002753900C', null);
INSERT INTO `mr_host` VALUES ('11407', 'OLD-4-3-4-1', 'OLD-4-3-4-1', '10.10.204.123', 'old080027C2F823', null);
INSERT INTO `mr_host` VALUES ('11408', 'OLD-3-5-3-2', 'OLD-3-5-3-2', '10.10.203.138', 'old080027522A97', null);
INSERT INTO `mr_host` VALUES ('11409', 'OLD-2-4-3-1', 'OLD-2-4-3-1', '10.10.202.130', 'old0800275D1CBE', null);
INSERT INTO `mr_host` VALUES ('11410', 'OLD-3-1-3-2', 'OLD-3-1-3-2', '10.10.203.106', 'old080027DE246C', null);
INSERT INTO `mr_host` VALUES ('11412', 'OLD-1-1-2-2', 'OLD-1-1-2-2', '10.10.201.107', 'old080027276A54', null);
INSERT INTO `mr_host` VALUES ('11414', 'OLD-3-2-2-1', 'OLD-3-2-2-1', '10.10.203.111', 'old0800270E9DE3', null);
INSERT INTO `mr_host` VALUES ('11415', 'OLD-2-3-3-2', 'OLD-2-3-3-2', '10.10.202.122', 'old080027A631F7', null);
INSERT INTO `mr_host` VALUES ('11416', 'OLD-3-3-2-1', 'OLD-3-3-2-1', '10.10.203.120', 'old0800277B8085', null);
INSERT INTO `mr_host` VALUES ('11418', 'OLD-3-3-3-1', 'OLD-3-3-3-1', '10.10.203.122', 'old080027AB4205', null);
INSERT INTO `mr_host` VALUES ('11419', 'OLD-4-5-1-1', 'OLD-4-5-1-1', '10.10.204.133', 'old080027CA957B', null);
INSERT INTO `mr_host` VALUES ('11420', 'OLD-4-3-2-2', 'OLD-4-3-2-2', '10.10.204.120', 'old080027AB6698', null);
INSERT INTO `mr_host` VALUES ('11422', 'OLD-4-1-2-2', 'OLD-4-1-2-2', '10.10.204.104', 'old08002770A017', null);
INSERT INTO `mr_host` VALUES ('11423', 'OLD-1-4-3-2', 'OLD-1-4-3-2', '10.10.201.130', 'old0800270E6BA3', null);
INSERT INTO `mr_host` VALUES ('11425', 'OLD-2-4-2-1', 'OLD-2-4-2-1', '10.10.202.128', 'old080027794FD7', null);
INSERT INTO `mr_host` VALUES ('11427', 'OLD-1-5-3-1', 'OLD-1-5-3-1', '10.10.201.137', 'old08002721735F', null);
INSERT INTO `mr_host` VALUES ('11429', 'OLD-2-1-2-1', 'OLD-2-1-2-1', '10.10.202.104', 'old080027C8AACB', null);
INSERT INTO `mr_host` VALUES ('11431', 'OLD-4-1-2-1', 'OLD-4-1-2-1', '10.10.204.103', 'old08002716EA59', null);
INSERT INTO `mr_host` VALUES ('11433', 'OLD-4-5-2-2', 'OLD-4-5-2-2', '10.10.204.135', 'old0800277462E8', null);
INSERT INTO `mr_host` VALUES ('11435', 'OLD-3-1-3-1', 'OLD-3-1-3-1', '10.10.203.105', 'old08002710DACC', null);
INSERT INTO `mr_host` VALUES ('11437', 'OLD-4-3-1-1', 'OLD-4-3-1-1', '10.10.204.118', 'old080027722C97', null);
INSERT INTO `mr_host` VALUES ('11439', 'OLD-3-2-2-2', 'OLD-3-2-2-2', '10.10.203.112', 'old0800274EBA36', null);
INSERT INTO `mr_host` VALUES ('11440', 'OLD-2-2-3-1', 'OLD-2-2-3-1', '10.10.202.113', 'old080027A207FF', null);
INSERT INTO `mr_host` VALUES ('11441', 'OLD-1-4-3-1', 'OLD-1-4-3-1', '10.10.201.129', 'old08002777DED0', null);
INSERT INTO `mr_host` VALUES ('11444', 'OLD-2-5-2-2', 'OLD-2-5-2-2', '10.10.202.136', 'old080027BFCFDF', null);
INSERT INTO `mr_host` VALUES ('11446', 'OLD-3-3-3-2', 'OLD-3-3-3-2', '10.10.203.121', 'old08002790079C', null);
INSERT INTO `mr_host` VALUES ('11450', 'OLD-3-2-3-2', 'OLD-3-2-3-2', '10.10.203.114', 'old080027A337AF', null);
INSERT INTO `mr_host` VALUES ('11451', 'OLD-4-1-1-1', 'OLD-4-1-1-1', '10.10.204.102', 'old080027273A0E', null);
INSERT INTO `mr_host` VALUES ('11459', 'OLD-2-4-2-2', 'OLD-2-4-2-2', '10.10.202.127', 'old080027A3F1E8', null);
INSERT INTO `mr_host` VALUES ('11460', 'OLD-4-2-2-1', 'OLD-4-2-2-1', '10.10.204.112', 'old0800274033F9', null);
INSERT INTO `mr_host` VALUES ('11462', 'OLD-1-5-2-1', 'OLD-1-5-2-1', '10.10.201.135', 'old08002731FD42', null);
INSERT INTO `mr_host` VALUES ('11464', 'OLD-3-4-2-2', 'OLD-3-4-2-2', '10.10.203.128', 'old080027C17F02', null);
INSERT INTO `mr_host` VALUES ('11466', 'OLD-4-2-1-1', 'OLD-4-2-1-1', '10.10.204.109', 'old080027F7659B', null);
INSERT INTO `mr_host` VALUES ('11468', 'OLD-2-4-3-2', 'OLD-2-4-3-2', '10.10.202.129', 'old0800270EADC7', null);
INSERT INTO `mr_host` VALUES ('11470', 'OLD-4-5-2-1', 'OLD-4-5-2-1', '10.10.204.136', 'old080027017DF5', null);
INSERT INTO `mr_host` VALUES ('11472', 'OLD-1-5-2-2', 'OLD-1-5-2-2', '10.10.201.136', 'old0800276F5086', null);
INSERT INTO `mr_host` VALUES ('11474', 'OLD-1-1-2-1', 'OLD-1-1-2-1', '10.10.201.108', 'old080027B929E0', null);
INSERT INTO `mr_host` VALUES ('11475', 'OLD-2-2-1-1', 'OLD-2-2-1-1', '10.10.202.110', 'old08002795F97E', null);
INSERT INTO `mr_host` VALUES ('11477', 'OLD-4-2-1-2', 'OLD-4-2-1-2', '10.10.204.110', 'old080027B99898', null);
INSERT INTO `mr_host` VALUES ('11479', 'OLD-3-1-2-1', 'OLD-3-1-2-1', '10.10.203.103', 'old080027170D70', null);
INSERT INTO `mr_host` VALUES ('11481', 'OLD-4-4-1-1', 'OLD-4-4-1-1', '10.10.204.125', 'old08002786E5C8', null);
INSERT INTO `mr_host` VALUES ('11483', 'OLD-2-3-2-1', 'OLD-2-3-2-1', '10.10.202.119', 'old08002713B527', null);
INSERT INTO `mr_host` VALUES ('11485', 'OLD-4-2-2-2', 'OLD-4-2-2-2', '10.10.204.111', 'old08002765F5BA', null);
INSERT INTO `mr_host` VALUES ('11487', 'OLD-1-5-3-2', 'OLD-1-5-3-2', '10.10.201.138', 'old0800271C82FC', null);
INSERT INTO `mr_host` VALUES ('11489', 'OLD-1-3-3-2', 'OLD-1-3-3-2', '10.10.201.121', 'old080027BB8F4C', null);
INSERT INTO `mr_host` VALUES ('11491', 'OLD-1-3-4-1', 'OLD-1-3-4-1', '10.10.201.124', 'old0800275B3593', null);
INSERT INTO `mr_host` VALUES ('11493', 'OLD-3-3-2-2', 'OLD-3-3-2-2', '10.10.203.119', 'old080027A640AF', null);
INSERT INTO `mr_host` VALUES ('11495', 'OLD-4-4-4-2', 'OLD-4-4-4-2', '10.10.204.131', 'old080027BB6313', null);
INSERT INTO `mr_host` VALUES ('11497', 'OLD-4-1-1-2', 'OLD-4-1-1-2', '10.10.204.101', 'old0800271C305B', null);
INSERT INTO `mr_host` VALUES ('11498', 'OLD-2-2-2-1', 'OLD-2-2-2-1', '10.10.202.112', 'old0800274C0C96', null);
INSERT INTO `mr_host` VALUES ('11499', 'OLD-4-4-1-2', 'OLD-4-4-1-2', '10.10.204.126', 'old080027B0F926', null);
INSERT INTO `mr_host` VALUES ('11500', 'OLD-1-3-2-1', 'OLD-1-3-2-1', '10.10.201.119', 'old080027998ED2', null);
INSERT INTO `mr_host` VALUES ('11503', 'OLD-2-5-2-1', 'OLD-2-5-2-1', '10.10.202.135', 'old0800275048CE', null);
INSERT INTO `mr_host` VALUES ('11505', 'OLD-4-5-1-2', 'OLD-4-5-1-2', '10.10.204.134', 'old0800273E6ACD', null);
INSERT INTO `mr_host` VALUES ('11507', 'OLD-2-3-2-2', 'OLD-2-3-2-2', '10.10.202.120', 'old0800279CDA51', null);
INSERT INTO `mr_host` VALUES ('11509', 'OLD-2-2-3-2', 'OLD-2-2-3-2', '10.10.202.114', 'old08002733B25A', null);
INSERT INTO `mr_host` VALUES ('11511', 'OLD-1-2-2-1', 'OLD-1-2-2-1', '10.10.201.113', 'old080027332E8D', null);
INSERT INTO `mr_host` VALUES ('11513', 'OLD-1-1-3-1', 'OLD-1-1-3-1', '10.10.201.109', 'old0800271ABC4B', null);
INSERT INTO `mr_host` VALUES ('11515', 'OLD-2-4-4-2', 'OLD-2-4-4-2', '10.10.202.131', 'old080027FF5284', null);
INSERT INTO `mr_host` VALUES ('11517', 'OLD-2-1-2-2', 'OLD-2-1-2-2', '10.10.202.103', 'old0800275BD820', null);
INSERT INTO `mr_host` VALUES ('11519', 'OLD-1-5-4-2', 'OLD-1-5-4-2', '10.10.201.140', 'old080027961E90', null);
INSERT INTO `mr_host` VALUES ('11521', 'OLD-2-4-4-1', 'OLD-2-4-4-1', '10.10.202.132', 'old080027E58C05', null);
INSERT INTO `mr_host` VALUES ('11525', 'OLD-3-5-2-1', 'OLD-3-5-2-1', '10.10.203.135', 'old0800274A431F', null);
INSERT INTO `mr_host` VALUES ('11527', 'OLD-1-3-2-2', 'OLD-1-3-2-2', '10.10.201.120', 'old080027E5B5BA', null);
INSERT INTO `mr_host` VALUES ('11529', 'OLD-1-4-2-1', 'OLD-1-4-2-1', '10.10.201.127', 'old0800270C0EA8', null);
INSERT INTO `mr_host` VALUES ('11531', 'OLD-3-5-2-2', 'OLD-3-5-2-2', '10.10.203.136', 'old0800279D27B5', null);
INSERT INTO `mr_host` VALUES ('11533', 'OLD-1-1-4-2', 'OLD-1-1-4-2', '10.10.201.111', 'old0800279F6BDC', null);
INSERT INTO `mr_host` VALUES ('11535', 'OLD-3-4-1-2', 'OLD-3-4-1-2', '10.10.203.126', 'old080027158246', null);
INSERT INTO `mr_host` VALUES ('11537', 'OLD-3-4-4-1', 'OLD-3-4-4-1', '10.10.203.131', 'old0800275668B5', null);
INSERT INTO `mr_host` VALUES ('11539', 'OLD-2-2-4-1', 'OLD-2-2-4-1', '10.10.202.116', 'old0800276C8E95', null);
INSERT INTO `mr_host` VALUES ('11541', 'OLD-3-4-4-2', 'OLD-3-4-4-2', '10.10.203.132', 'old0800271BB437', null);
INSERT INTO `mr_host` VALUES ('11543', 'OLD-2-1-4-2', 'OLD-2-1-4-2', '10.10.202.107', 'old080027789636', null);
INSERT INTO `mr_host` VALUES ('11545', 'OLD-2-1-3-2', 'OLD-2-1-3-2', '10.10.202.106', 'old08002716E8BB', null);
INSERT INTO `mr_host` VALUES ('11547', 'OLD-3-1-4-1', 'OLD-3-1-4-1', '10.10.203.108', 'old080027720DC9', null);
INSERT INTO `mr_host` VALUES ('11549', 'OLD-1-1-1-2', 'OLD-1-1-1-2', '10.10.201.101', 'old080027E8DFC3', null);
INSERT INTO `mr_host` VALUES ('11551', 'OLD-3-3-4-2', 'OLD-3-3-4-2', '10.10.203.123', 'old080027C4E92B', null);
INSERT INTO `mr_host` VALUES ('11553', 'OLD-1-1-4-1', 'OLD-1-1-4-1', '10.10.201.112', 'old080027224C8E', null);
INSERT INTO `mr_host` VALUES ('11557', 'OLD-4-4-4-1', 'OLD-4-4-4-1', '10.10.204.132', 'old080027A8190B', null);
INSERT INTO `mr_host` VALUES ('11561', 'OLD-3-2-1-1', 'OLD-3-2-1-1', '10.10.203.109', 'old080027F70711', null);
INSERT INTO `mr_host` VALUES ('11565', 'OLD-3-3-1-1', 'OLD-3-3-1-1', '10.10.203.118', 'old080027AA9E30', null);
INSERT INTO `mr_host` VALUES ('11569', 'OLD-3-5-4-1', 'OLD-3-5-4-1', '10.10.203.139', 'old080027606181', null);
INSERT INTO `mr_host` VALUES ('11571', 'OLD-3-1-1-2', 'OLD-3-1-1-2', '10.10.203.102', 'old08002762B0A1', null);
INSERT INTO `mr_host` VALUES ('11573', 'OLD-1-3-4-2', 'OLD-1-3-4-2', '10.10.201.123', 'old080027AAA8A3', null);
INSERT INTO `mr_host` VALUES ('11575', 'OLD-2-3-1-1', 'OLD-2-3-1-1', '10.10.202.118', 'old0800279FDEB4', null);
INSERT INTO `mr_host` VALUES ('11577', 'OLD-2-3-4-2', 'OLD-2-3-4-2', '10.10.202.123', 'old080027C3C451', null);
INSERT INTO `mr_host` VALUES ('11579', 'OLD-3-3-4-1', 'OLD-3-3-4-1', '10.10.203.124', 'old080027DCA047', null);
INSERT INTO `mr_host` VALUES ('11581', 'OLD-1-2-4-1', 'OLD-1-2-4-1', '10.10.201.117', 'old0800277F6D15', null);
INSERT INTO `mr_host` VALUES ('11583', 'OLD-2-3-1-2', 'OLD-2-3-1-2', '10.10.202.117', 'old0800275375CD', null);
INSERT INTO `mr_host` VALUES ('11585', 'OLD-1-5-1-1', 'OLD-1-5-1-1', '10.10.201.133', 'old080027A83B11', null);
INSERT INTO `mr_host` VALUES ('11587', 'OLD-2-4-1-1', 'OLD-2-4-1-1', '10.10.202.126', 'old080027AB9A89', null);
INSERT INTO `mr_host` VALUES ('11591', 'OLD-2-3-4-1', 'OLD-2-3-4-1', '10.10.202.124', 'old080027D6217B', null);
INSERT INTO `mr_host` VALUES ('11593', 'OLD-1-3-1-1', 'OLD-1-3-1-1', '10.10.201.106', 'old080027B001A6', null);
INSERT INTO `mr_host` VALUES ('11595', 'OLD-2-2-4-2', 'OLD-2-2-4-2', '10.10.202.115', 'old0800273DA0A2', null);
INSERT INTO `mr_host` VALUES ('11597', 'OLD-3-2-1-2', 'OLD-3-2-1-2', '10.10.203.110', 'old080027DFB8E8', null);
INSERT INTO `mr_host` VALUES ('11599', 'OLD-2-4-1-2', 'OLD-2-4-1-2', '10.10.202.125', 'old0800277E436B', null);
INSERT INTO `mr_host` VALUES ('11601', 'OLD-1-4-4-2', 'OLD-1-4-4-2', '10.10.201.132', 'old0800272367BA', null);
INSERT INTO `mr_host` VALUES ('11603', 'OLD-1-4-4-1', 'OLD-1-4-4-1', '10.10.201.131', 'old08002706FE99', null);
INSERT INTO `mr_host` VALUES ('11605', 'OLD-3-1-1-1', 'OLD-3-1-1-1', '10.10.203.101', 'old0800276A49FF', null);
INSERT INTO `mr_host` VALUES ('11607', 'OLD-2-1-1-2', 'OLD-2-1-1-2', '10.10.202.101', 'old080027807D65', null);
INSERT INTO `mr_host` VALUES ('11609', 'OLD-2-2-1-2', 'OLD-2-2-1-2', '10.10.202.109', 'old080027ACF4F1', null);
INSERT INTO `mr_host` VALUES ('11613', 'OLD-3-4-1-1', 'OLD-3-4-1-1', '10.10.203.125', 'old080027146AF3', null);
INSERT INTO `mr_host` VALUES ('11615', 'OLD-2-1-4-1', 'OLD-2-1-4-1', '10.10.202.108', 'old080027F97826', null);
INSERT INTO `mr_host` VALUES ('11617', 'OLD-2-5-1-2', 'OLD-2-5-1-2', '10.10.202.134', 'old0800271A160E', null);
INSERT INTO `mr_host` VALUES ('11619', 'OLD-1-5-1-2', 'OLD-1-5-1-2', '10.10.201.134', 'old08002794AF1B', null);
INSERT INTO `mr_host` VALUES ('11623', 'OLD-3-3-1-2', 'OLD-3-3-1-2', '10.10.203.117', 'old080027ED2BBA', null);
INSERT INTO `mr_host` VALUES ('11625', 'OLD-3-5-4-2', 'OLD-3-5-4-2', '10.10.203.140', 'old0800277AB267', null);
INSERT INTO `mr_host` VALUES ('11627', 'OLD-1-4-1-1', 'OLD-1-4-1-1', '10.10.201.126', 'old080027CED61A', null);
INSERT INTO `mr_host` VALUES ('11629', 'OLD-1-4-1-2', 'OLD-1-4-1-2', '10.10.201.125', 'old080027716D9B', null);
INSERT INTO `mr_host` VALUES ('11631', 'OLD-3-5-1-1', 'OLD-3-5-1-1', '10.10.203.134', 'old08002769DE31', null);
INSERT INTO `mr_host` VALUES ('11633', 'OLD-3-5-1-2', 'OLD-3-5-1-2', '10.10.203.133', 'old080027C0CE51', null);
INSERT INTO `mr_host` VALUES ('11635', 'OLD-1-2-1-2', 'OLD-1-2-1-2', '10.10.201.104', 'old08002724E6A1', null);
INSERT INTO `mr_host` VALUES ('11637', 'OLD-2-5-1-1', 'OLD-2-5-1-1', '10.10.202.133', 'old08002774F76E', null);
INSERT INTO `mr_host` VALUES ('11638', 'OLD-2-1-1-1', 'OLD-2-1-1-1', '10.10.202.102', 'old080027151C4E', null);
INSERT INTO `mr_host` VALUES ('11639', 'OLD-2-5-4-2', 'OLD-2-5-4-2', '10.10.202.140', 'old0800276E5ACA', null);
INSERT INTO `mr_host` VALUES ('11640', 'OLD-2-5-4-1', 'OLD-2-5-4-1', '10.10.202.139', 'old080027E20FFE', null);
INSERT INTO `mr_host` VALUES ('11641', 'OLD-7-1-3-1', 'OLD-7-1-3-1', '10.10.207.106', 'old080027053A33', null);
INSERT INTO `mr_host` VALUES ('11642', 'OLD-6-3-3-2', 'OLD-6-3-3-2', '10.10.206.122', 'old080027AF9702', null);
INSERT INTO `mr_host` VALUES ('11644', 'OLD-6-3-4-1', 'OLD-6-3-4-1', '10.10.206.123', 'old08002740FFA6', null);
INSERT INTO `mr_host` VALUES ('11645', 'OLD-6-1-2-1', 'OLD-6-1-2-1', '10.10.206.104', 'old08002746F846', null);
INSERT INTO `mr_host` VALUES ('11646', 'OLD-7-5-3-1', 'OLD-7-5-3-1', '10.10.207.137', 'old080027A6AEFE', null);
INSERT INTO `mr_host` VALUES ('11647', 'OLD-6-1-4-2', 'OLD-6-1-4-2', '10.10.206.107', 'old080027C0C9B4', null);
INSERT INTO `mr_host` VALUES ('11649', 'OLD-6-5-1-2', 'OLD-6-5-1-2', '10.10.206.134', 'old080027844C78', null);
INSERT INTO `mr_host` VALUES ('11650', 'OLD-6-1-3-1', 'OLD-6-1-3-1', '10.10.206.106', 'old08002707A77C', null);
INSERT INTO `mr_host` VALUES ('11651', 'OLD-6-3-1-1', 'OLD-6-3-1-1', '10.10.206.118', 'old080027D1533E', null);
INSERT INTO `mr_host` VALUES ('11652', 'OLD-6-1-4-1', 'OLD-6-1-4-1', '10.10.206.108', 'old0800279A46D7', null);
INSERT INTO `mr_host` VALUES ('11653', 'OLD-6-1-2-2', 'OLD-6-1-2-2', '10.10.206.103', 'old080027DF70F1', null);
INSERT INTO `mr_host` VALUES ('11654', 'OLD-6-2-2-1', 'OLD-6-2-2-1', '10.10.206.112', 'old080027AA2545', null);
INSERT INTO `mr_host` VALUES ('11655', 'OLD-8-2-4-1', 'OLD-8-2-4-1', '10.10.208.115', 'old0800275B9FA3', null);
INSERT INTO `mr_host` VALUES ('11657', 'OLD-8-4-3-1', 'OLD-8-4-3-1', '10.10.208.130', 'old0800270BED53', null);
INSERT INTO `mr_host` VALUES ('11660', 'OLD-8-5-2-1', 'OLD-8-5-2-1', '10.10.208.136', 'old080027FCBA25', null);
INSERT INTO `mr_host` VALUES ('11661', 'OLD-8-5-2-2', 'OLD-8-5-2-2', '10.10.208.135', 'old080027B2FA4F', null);
INSERT INTO `mr_host` VALUES ('11663', 'OLD-8-2-2-2', 'OLD-8-2-2-2', '10.10.208.111', 'old080027B7E964', null);
INSERT INTO `mr_host` VALUES ('11665', 'OLD-8-2-4-2', 'OLD-8-2-4-2', '10.10.208.116', 'old0800270A9348', null);
INSERT INTO `mr_host` VALUES ('11667', 'OLD-8-1-1-1', 'OLD-8-1-1-1', '10.10.208.101', 'old08002798D7C6', null);
INSERT INTO `mr_host` VALUES ('11669', 'OLD-8-1-1-2', 'OLD-8-1-1-2', '10.10.208.102', 'old080027B4C3C5', null);
INSERT INTO `mr_host` VALUES ('11671', 'OLD-8-3-1-2', 'OLD-8-3-1-2', '10.10.208.117', 'old080027BEA97F', null);
INSERT INTO `mr_host` VALUES ('11672', 'OLD-8-2-3-2', 'OLD-8-2-3-2', '10.10.208.114', 'old080027C39ECF', null);
INSERT INTO `mr_host` VALUES ('11673', 'OLD-8-4-4-2', 'OLD-8-4-4-2', '10.10.208.131', 'old080027B4A151', null);
INSERT INTO `mr_host` VALUES ('11676', 'OLD-8-1-2-2', 'OLD-8-1-2-2', '10.10.208.104', 'old0800277F4D3D', null);
INSERT INTO `mr_host` VALUES ('11678', 'OLD-8-4-2-1', 'OLD-8-4-2-1', '10.10.208.127', 'old08002729F6D7', null);
INSERT INTO `mr_host` VALUES ('11681', 'OLD-8-5-4-2', 'OLD-8-5-4-2', '10.10.208.139', 'old08002773AA8E', null);
INSERT INTO `mr_host` VALUES ('11683', 'OLD-8-3-3-2', 'OLD-8-3-3-2', '10.10.208.122', 'old0800270ABFCB', null);
INSERT INTO `mr_host` VALUES ('11685', 'OLD-8-5-3-1', 'OLD-8-5-3-1', '10.10.208.137', 'old0800272358D5', null);
INSERT INTO `mr_host` VALUES ('11687', 'OLD-8-5-4-1', 'OLD-8-5-4-1', '10.10.208.140', 'old08002795F08D', null);
INSERT INTO `mr_host` VALUES ('11690', 'OLD-8-3-2-1', 'OLD-8-3-2-1', '10.10.208.120', 'old0800270E6F04', null);
INSERT INTO `mr_host` VALUES ('11691', 'OLD-5-5-3-2', 'OLD-5-5-3-2', '10.10.205.137', 'old080027365607', null);
INSERT INTO `mr_host` VALUES ('11692', 'OLD-8-1-3-1', 'OLD-8-1-3-1', '10.10.208.105', 'old080027A7AFA0', null);
INSERT INTO `mr_host` VALUES ('11693', 'OLD-6-3-1-2', 'OLD-6-3-1-2', '10.10.206.117', 'old080027AC7211', null);
INSERT INTO `mr_host` VALUES ('11694', 'OLD-8-3-1-1', 'OLD-8-3-1-1', '10.10.208.118', 'old080027E8DDBB', null);
INSERT INTO `mr_host` VALUES ('11695', 'OLD-8-5-3-2', 'OLD-8-5-3-2', '10.10.208.138', 'old0800273906AC', null);
INSERT INTO `mr_host` VALUES ('11697', 'OLD-8-1-3-2', 'OLD-8-1-3-2', '10.10.208.106', 'old080027DB2747', null);
INSERT INTO `mr_host` VALUES ('11698', 'OLD-8-1-4-1', 'OLD-8-1-4-1', '10.10.208.107', 'old080027715E23', null);
INSERT INTO `mr_host` VALUES ('11699', 'OLD-8-3-3-1', 'OLD-8-3-3-1', '10.10.208.121', 'old080027834986', null);
INSERT INTO `mr_host` VALUES ('11700', 'OLD-8-1-4-2', 'OLD-8-1-4-2', '10.10.208.108', 'old080027EE13E3', null);
INSERT INTO `mr_host` VALUES ('11701', 'OLD-6-5-3-2', 'OLD-6-5-3-2', '10.10.206.138', 'old0800279076C6', null);
INSERT INTO `mr_host` VALUES ('11702', 'OLD-5-3-2-2', 'OLD-5-3-2-2', '10.10.205.120', 'old0800278EFF7A', null);
INSERT INTO `mr_host` VALUES ('11703', 'OLD-8-3-2-2', 'OLD-8-3-2-2', '10.10.208.119', 'old08002753D0B2', null);
INSERT INTO `mr_host` VALUES ('11704', 'OLD-8-5-1-2', 'OLD-8-5-1-2', '10.10.208.133', 'old080027F8BD48', null);
INSERT INTO `mr_host` VALUES ('11705', 'OLD-8-4-1-2', 'OLD-8-4-1-2', '10.10.208.125', 'old0800276F03FC', null);
INSERT INTO `mr_host` VALUES ('11707', 'OLD-5-5-2-1', 'OLD-5-5-2-1', '10.10.205.136', 'old080027C7BE22', null);
INSERT INTO `mr_host` VALUES ('11708', 'OLD-8-4-4-1', 'OLD-8-4-4-1', '10.10.208.132', 'old08002741DA35', null);
INSERT INTO `mr_host` VALUES ('11710', 'OLD-7-4-2-1', 'OLD-7-4-2-1', '10.10.207.128', 'old0800270EA39C', null);
INSERT INTO `mr_host` VALUES ('11713', 'OLD-7-2-2-1', 'OLD-7-2-2-1', '10.10.207.112', 'old080027FCC315', null);
INSERT INTO `mr_host` VALUES ('11714', 'OLD-5-4-3-2', 'OLD-5-4-3-2', '10.10.205.129', 'old080027E867AB', null);
INSERT INTO `mr_host` VALUES ('11716', 'OLD-7-2-2-2', 'OLD-7-2-2-2', '10.10.207.111', 'old0800276A60B8', null);
INSERT INTO `mr_host` VALUES ('11717', 'OLD-7-5-3-2', 'OLD-7-5-3-2', '10.10.207.138', 'old0800279EA80D', null);
INSERT INTO `mr_host` VALUES ('11718', 'OLD-7-4-2-2', 'OLD-7-4-2-2', '10.10.207.127', 'old080027033F76', null);
INSERT INTO `mr_host` VALUES ('11719', 'OLD-7-2-3-2', 'OLD-7-2-3-2', '10.10.207.113', 'old080027D55007', null);
INSERT INTO `mr_host` VALUES ('11720', 'OLD-7-3-3-2', 'OLD-7-3-3-2', '10.10.207.121', 'old08002722F118', null);
INSERT INTO `mr_host` VALUES ('11721', 'OLD-7-1-2-1', 'OLD-7-1-2-1', '10.10.207.104', 'old080027539807', null);
INSERT INTO `mr_host` VALUES ('11722', 'OLD-7-1-2-2', 'OLD-7-1-2-2', '10.10.207.103', 'old080027B73874', null);
INSERT INTO `mr_host` VALUES ('11723', 'OLD-6-5-2-1', 'OLD-6-5-2-1', '10.10.206.136', 'old0800274C064E', null);
INSERT INTO `mr_host` VALUES ('11724', 'OLD-6-5-3-1', 'OLD-6-5-3-1', '10.10.206.137', 'old080027BA9479', null);
INSERT INTO `mr_host` VALUES ('11725', 'OLD-7-2-3-1', 'OLD-7-2-3-1', '10.10.207.114', 'old080027407D0D', null);
INSERT INTO `mr_host` VALUES ('11727', 'OLD-6-5-1-1', 'OLD-6-5-1-1', '10.10.206.133', 'old08002719110D', null);
INSERT INTO `mr_host` VALUES ('11728', 'OLD-7-3-4-2', 'OLD-7-3-4-2', '10.10.207.124', 'old08002702B57A', null);
INSERT INTO `mr_host` VALUES ('11729', 'OLD-6-5-4-1', 'OLD-6-5-4-1', '10.10.206.140', 'old080027E99725', null);
INSERT INTO `mr_host` VALUES ('11731', 'OLD-6-5-4-2', 'OLD-6-5-4-2', '10.10.206.139', 'old08002735C9EA', null);
INSERT INTO `mr_host` VALUES ('11733', 'OLD-7-3-2-1', 'OLD-7-3-2-1', '10.10.207.119', 'old08002798BF64', null);
INSERT INTO `mr_host` VALUES ('11735', 'OLD-7-4-4-1', 'OLD-7-4-4-1', '10.10.207.131', 'old080027BD47B1', null);
INSERT INTO `mr_host` VALUES ('11736', 'OLD-6-4-4-2', 'OLD-6-4-4-2', '10.10.206.131', 'old0800279EEA0A', null);
INSERT INTO `mr_host` VALUES ('11737', 'OLD-7-2-1-2', 'OLD-7-2-1-2', '10.10.207.110', 'old080027D76755', null);
INSERT INTO `mr_host` VALUES ('11738', 'OLD-7-4-3-2', 'OLD-7-4-3-2', '10.10.207.130', 'old080027EFF09B', null);
INSERT INTO `mr_host` VALUES ('11739', 'OLD-5-5-1-1', 'OLD-5-5-1-1', '10.10.205.134', 'old080027415422', null);
INSERT INTO `mr_host` VALUES ('11740', 'OLD-7-5-2-2', 'OLD-7-5-2-2', '10.10.207.135', 'old080027AB70FB', null);
INSERT INTO `mr_host` VALUES ('11741', 'OLD-7-4-4-2', 'OLD-7-4-4-2', '10.10.207.132', 'old080027974E91', null);
INSERT INTO `mr_host` VALUES ('11742', 'OLD-6-3-3-1', 'OLD-6-3-3-1', '10.10.206.121', 'old080027427F66', null);
INSERT INTO `mr_host` VALUES ('11743', 'OLD-7-3-1-2', 'OLD-7-3-1-2', '10.10.207.118', 'old080027CD9A49', null);
INSERT INTO `mr_host` VALUES ('11745', 'OLD-7-3-4-1', 'OLD-7-3-4-1', '10.10.207.123', 'old0800277AF5BE', null);
INSERT INTO `mr_host` VALUES ('11747', 'OLD-7-3-1-1', 'OLD-7-3-1-1', '10.10.207.117', 'old08002750C0DB', null);
INSERT INTO `mr_host` VALUES ('11749', 'OLD-7-5-2-1', 'OLD-7-5-2-1', '10.10.207.136', 'old080027D8A76D', null);
INSERT INTO `mr_host` VALUES ('11751', 'OLD-7-3-3-1', 'OLD-7-3-3-1', '10.10.207.122', 'old08002759F4B6', null);
INSERT INTO `mr_host` VALUES ('11753', 'OLD-7-1-3-2', 'OLD-7-1-3-2', '10.10.207.105', 'old080027C5914D', null);
INSERT INTO `mr_host` VALUES ('11754', 'OLD-7-5-1-2', 'OLD-7-5-1-2', '10.10.207.134', 'old0800273E0C5C', null);
INSERT INTO `mr_host` VALUES ('11755', 'OLD-7-1-4-2', 'OLD-7-1-4-2', '10.10.207.107', 'old080027106D96', null);
INSERT INTO `mr_host` VALUES ('11757', 'OLD-7-4-1-1', 'OLD-7-4-1-1', '10.10.207.126', 'old0800270E6DBB', null);
INSERT INTO `mr_host` VALUES ('11759', 'OLD-7-2-1-1', 'OLD-7-2-1-1', '10.10.207.109', 'old080027F6C8DA', null);
INSERT INTO `mr_host` VALUES ('11760', 'OLD-7-4-3-1', 'OLD-7-4-3-1', '10.10.207.129', 'old080027D2E855', null);
INSERT INTO `mr_host` VALUES ('11761', 'OLD-5-3-2-1', 'OLD-5-3-2-1', '10.10.205.119', 'old080027E936D6', null);
INSERT INTO `mr_host` VALUES ('11763', 'OLD-5-1-3-2', 'OLD-5-1-3-2', '10.10.205.105', 'old080027954B44', null);
INSERT INTO `mr_host` VALUES ('11765', 'OLD-5-1-4-1', 'OLD-5-1-4-1', '10.10.205.107', 'old08002788B195', null);
INSERT INTO `mr_host` VALUES ('11767', 'OLD-7-1-1-1', 'OLD-7-1-1-1', '10.10.207.102', 'old0800275352CA', null);
INSERT INTO `mr_host` VALUES ('11768', 'OLD-5-2-2-2', 'OLD-5-2-2-2', '10.10.205.111', 'old080027C5368B', null);
INSERT INTO `mr_host` VALUES ('11770', 'OLD-5-3-3-2', 'OLD-5-3-3-2', '10.10.205.121', 'old08002727AA2A', null);
INSERT INTO `mr_host` VALUES ('11772', 'OLD-5-1-1-1', 'OLD-5-1-1-1', '10.10.205.101', 'old080027E6D6A9', null);
INSERT INTO `mr_host` VALUES ('11773', 'OLD-5-1-2-1', 'OLD-5-1-2-1', '10.10.205.103', 'old08002753C419', null);
INSERT INTO `mr_host` VALUES ('11774', 'OLD-5-1-3-1', 'OLD-5-1-3-1', '10.10.205.106', 'old0800272A6E89', null);
INSERT INTO `mr_host` VALUES ('11775', 'OLD-5-4-4-2', 'OLD-5-4-4-2', '10.10.205.132', 'old080027C8BFA4', null);
INSERT INTO `mr_host` VALUES ('11776', 'OLD-5-4-2-2', 'OLD-5-4-2-2', '10.10.205.127', 'old08002765B525', null);
INSERT INTO `mr_host` VALUES ('11778', 'OLD-5-1-4-2', 'OLD-5-1-4-2', '10.10.205.108', 'old08002799196B', null);
INSERT INTO `mr_host` VALUES ('11779', 'OLD-5-4-1-2', 'OLD-5-4-1-2', '10.10.205.126', 'old080027C60F7B', null);
INSERT INTO `mr_host` VALUES ('11780', 'OLD-5-2-3-1', 'OLD-5-2-3-1', '10.10.205.113', 'old080027713729', null);
INSERT INTO `mr_host` VALUES ('11781', 'OLD-5-2-1-1', 'OLD-5-2-1-1', '10.10.205.110', 'old0800274B3BCF', null);
INSERT INTO `mr_host` VALUES ('11782', 'OLD-5-3-4-2', 'OLD-5-3-4-2', '10.10.205.124', 'old0800279DB720', null);
INSERT INTO `mr_host` VALUES ('11783', 'OLD-5-3-4-1', 'OLD-5-3-4-1', '10.10.205.123', 'old0800276F4445', null);
INSERT INTO `mr_host` VALUES ('11785', 'OLD-5-2-3-2', 'OLD-5-2-3-2', '10.10.205.114', 'old080027731D40', null);
INSERT INTO `mr_host` VALUES ('11789', 'OLD-6-4-4-1', 'OLD-6-4-4-1', '10.10.206.132', 'old0800273464A7', null);
INSERT INTO `mr_host` VALUES ('11790', 'OLD-5-4-1-1', 'OLD-5-4-1-1', '10.10.205.125', 'old0800276CABFA', null);
INSERT INTO `mr_host` VALUES ('11791', 'OLD-6-4-3-1', 'OLD-6-4-3-1', '10.10.206.130', 'old0800273B868C', null);
INSERT INTO `mr_host` VALUES ('11793', 'OLD-5-3-1-1', 'OLD-5-3-1-1', '10.10.205.118', 'old08002700F858', null);
INSERT INTO `mr_host` VALUES ('11794', 'OLD-5-2-4-1', 'OLD-5-2-4-1', '10.10.205.116', 'old0800271CA0E6', null);
INSERT INTO `mr_host` VALUES ('11796', 'OLD-5-3-1-2', 'OLD-5-3-1-2', '10.10.205.117', 'old08002709C1DC', null);
INSERT INTO `mr_host` VALUES ('11797', 'OLD-6-2-4-2', 'OLD-6-2-4-2', '10.10.206.115', 'old0800275C065E', null);
INSERT INTO `mr_host` VALUES ('11800', 'OLD-6-2-1-2', 'OLD-6-2-1-2', '10.10.206.109', 'old080027A63244', null);
INSERT INTO `mr_host` VALUES ('11801', 'OLD-6-4-3-2', 'OLD-6-4-3-2', '10.10.206.129', 'old080027DC74A6', null);
INSERT INTO `mr_host` VALUES ('11802', 'OLD-6-4-2-1', 'OLD-6-4-2-1', '10.10.206.128', 'old080027249540', null);
INSERT INTO `mr_host` VALUES ('11803', 'OLD-6-1-1-1', 'OLD-6-1-1-1', '10.10.206.101', 'old080027856836', null);
INSERT INTO `mr_host` VALUES ('11804', 'OLD-6-4-2-2', 'OLD-6-4-2-2', '10.10.206.127', 'old0800276AE6A4', null);
INSERT INTO `mr_host` VALUES ('11805', 'OLD-6-2-4-1', 'OLD-6-2-4-1', '10.10.206.116', 'old080027503EF5', null);
INSERT INTO `mr_host` VALUES ('11806', 'OLD-5-4-4-1', 'OLD-5-4-4-1', '10.10.205.131', 'old08002773B0F0', null);
INSERT INTO `mr_host` VALUES ('11807', 'OLD-6-4-1-1', 'OLD-6-4-1-1', '10.10.206.126', 'old08002783ED15', null);
INSERT INTO `mr_host` VALUES ('11808', 'OLD-6-2-3-2', 'OLD-6-2-3-2', '10.10.206.114', 'old08002760F346', null);
INSERT INTO `mr_host` VALUES ('11809', 'OLD-5-5-3-1', 'OLD-5-5-3-1', '10.10.205.138', 'old080027761BFB', null);
INSERT INTO `mr_host` VALUES ('11810', 'OLD-8-2-1-1', 'OLD-8-2-1-1', '10.10.208.110', 'old080027BB9468', null);
INSERT INTO `mr_host` VALUES ('11811', 'OLD-5-5-2-2', 'OLD-5-5-2-2', '10.10.205.135', 'old080027C549EC', null);
INSERT INTO `mr_host` VALUES ('11812', 'OLD-8-4-2-2', 'OLD-8-4-2-2', '10.10.208.128', 'old08002706C48F', null);
INSERT INTO `mr_host` VALUES ('11813', 'OLD-8-4-1-1', 'OLD-8-4-1-1', '10.10.208.126', 'old08002731A10F', null);
INSERT INTO `mr_host` VALUES ('11814', 'OLD-8-5-1-1', 'OLD-8-5-1-1', '10.10.208.134', 'old0800277ACE55', null);
INSERT INTO `mr_host` VALUES ('11815', 'OLD-8-3-4-1', 'OLD-8-3-4-1', '10.10.208.123', 'old08002708AD8F', null);
INSERT INTO `mr_host` VALUES ('11816', 'OLD-7-2-4-1', 'OLD-7-2-4-1', '10.10.207.116', 'old080027B4E438', null);
INSERT INTO `mr_host` VALUES ('11817', 'OLD-7-5-1-1', 'OLD-7-5-1-1', '10.10.207.133', 'old080027CD4664', null);
INSERT INTO `mr_host` VALUES ('11818', 'OLD-7-5-4-1', 'OLD-7-5-4-1', '10.10.207.140', 'old080027CF44AA', null);
INSERT INTO `mr_host` VALUES ('11819', 'OLD-6-3-4-2', 'OLD-6-3-4-2', '10.10.206.124', 'old080027C234D6', null);
INSERT INTO `mr_host` VALUES ('11820', 'OLD-5-5-4-1', 'OLD-5-5-4-1', '10.10.205.140', 'old080027311B7A', null);
INSERT INTO `mr_host` VALUES ('11823', 'OLD-6-1-3-2', 'OLD-6-1-3-2', '10.10.206.105', 'old0800277BC60C', null);
INSERT INTO `mr_host` VALUES ('11825', 'OLD-5-5-4-2', 'OLD-5-5-4-2', '10.10.205.139', 'old08002793BC6F', null);
INSERT INTO `mr_host` VALUES ('11826', 'OLD-7-3-2-2', 'OLD-7-3-2-2', '10.10.207.120', 'old080027DB31DB', null);
INSERT INTO `mr_host` VALUES ('11827', 'OLD-5-1-1-2', 'OLD-5-1-1-2', '10.10.205.102', 'old0800271A0181', null);
INSERT INTO `mr_host` VALUES ('11828', 'OLD-7-5-4-2', 'OLD-7-5-4-2', '10.10.207.139', 'old0800275B1476', null);
INSERT INTO `mr_host` VALUES ('11829', 'OLD-7-1-4-1', 'OLD-7-1-4-1', '10.10.207.108', 'old080027DBBCE5', null);
INSERT INTO `mr_host` VALUES ('11831', 'OLD-7-4-1-2', 'OLD-7-4-1-2', '10.10.207.125', 'old0800275534E5', null);
INSERT INTO `mr_host` VALUES ('11832', 'OLD-5-4-3-1', 'OLD-5-4-3-1', '10.10.205.130', 'old080027524631', null);
INSERT INTO `mr_host` VALUES ('11834', 'OLD-7-2-4-2', 'OLD-7-2-4-2', '10.10.207.115', 'old080027789CA4', null);
INSERT INTO `mr_host` VALUES ('11835', 'OLD-5-2-2-1', 'OLD-5-2-2-1', '10.10.205.112', 'old08002709EAA8', null);
INSERT INTO `mr_host` VALUES ('11836', 'OLD-6-4-1-2', 'OLD-6-4-1-2', '10.10.206.125', 'old08002738C55E', null);
INSERT INTO `mr_host` VALUES ('11837', 'OLD-7-1-1-2', 'OLD-7-1-1-2', '10.10.207.101', 'old08002774E5C4', null);
INSERT INTO `mr_host` VALUES ('11838', 'OLD-6-5-2-2', 'OLD-6-5-2-2', '10.10.206.135', 'old080027DE487F', null);
INSERT INTO `mr_host` VALUES ('11839', 'SERVER-OLD', 'SERVER-OLD', '10.10.250.250', 'Server179', null);
INSERT INTO `mr_host` VALUES ('11840', 'OLD-8-1-2-1', 'OLD-8-1-2-1', '10.10.208.103', 'old08002749CB7B', null);
INSERT INTO `mr_host` VALUES ('11841', 'OLD-1-1-1-1', 'OLD-1-1-1-1', '10.10.201.102', 'old080027EE5925', null);
INSERT INTO `mr_host` VALUES ('11842', 'OLD-1-2-1-1', 'OLD-1-2-1-1', '10.10.201.103', 'old080027D73795', null);
INSERT INTO `mr_host` VALUES ('11843', 'OLD-1-2-4-2', 'OLD-1-2-4-2', '10.10.201.118', 'old080027FEC857', null);
INSERT INTO `mr_host` VALUES ('11844', 'OLD-1-3-1-2', 'OLD-1-3-1-2', '10.10.201.105', 'old080027B9CB5C', null);
INSERT INTO `mr_host` VALUES ('11845', 'OLD-1-4-2-2', 'OLD-1-4-2-2', '10.10.201.128', 'old080027C82FE8', null);
INSERT INTO `mr_host` VALUES ('11846', 'OLD-1-3-3-1', 'OLD-1-3-3-1', '10.10.201.122', 'old0800276CA03F', null);
INSERT INTO `mr_host` VALUES ('11847', 'OLD-1-2-2-2', 'OLD-1-2-2-2', '10.10.201.114', 'old0800272F0DE3', null);
INSERT INTO `mr_host` VALUES ('11848', 'OLD-3-5-3-1', 'OLD-3-5-3-1', '10.10.203.137', 'old08002796B693', null);
INSERT INTO `mr_host` VALUES ('11850', 'OLD-3-2-4-1', 'OLD-3-2-4-1', '10.10.203.116', 'old0800272036B6', null);
INSERT INTO `mr_host` VALUES ('11852', 'OLD-3-4-3-2', 'OLD-3-4-3-2', '10.10.203.129', 'old0800277A26BD', null);
INSERT INTO `mr_host` VALUES ('11853', 'OLD-4-1-3-1', 'OLD-4-1-3-1', '10.10.204.105', 'old080027DB4144', null);
INSERT INTO `mr_host` VALUES ('11854', 'OLD-3-2-4-2', 'OLD-3-2-4-2', '10.10.203.115', 'old080027FAFD2E', null);
INSERT INTO `mr_host` VALUES ('11855', 'OLD-3-1-4-2', 'OLD-3-1-4-2', '10.10.203.107', 'old080027207600', null);
INSERT INTO `mr_host` VALUES ('11856', 'OLD-1-5-4-1', 'OLD-1-5-4-1', '10.10.201.139', 'old080027B5704A', null);
INSERT INTO `mr_host` VALUES ('11857', 'OLD-5-1-2-2', 'OLD-5-1-2-2', '10.10.205.104', 'old080027F0FBBA', null);
INSERT INTO `mr_host` VALUES ('11858', 'OLD-4-5-3-1', 'OLD-4-5-3-1', '10.10.204.138', 'old0800273FE152', null);
INSERT INTO `mr_host` VALUES ('11859', 'OLD-6-3-2-1', 'OLD-6-3-2-1', '10.10.206.119', 'old0800271C2535', null);
INSERT INTO `mr_host` VALUES ('11861', 'OLD-5-5-1-2', 'OLD-5-5-1-2', '10.10.205.133', 'old080027C16FEE', null);
INSERT INTO `mr_host` VALUES ('11863', 'OLD-5-3-3-1', 'OLD-5-3-3-1', '10.10.205.122', 'old080027EBA277', null);
INSERT INTO `mr_host` VALUES ('11865', 'OLD-5-2-4-2', 'OLD-5-2-4-2', '10.10.205.115', 'old0800276EB34D', null);
INSERT INTO `mr_host` VALUES ('11867', 'OLD-6-1-1-2', 'OLD-6-1-1-2', '10.10.206.102', 'old0800270D3ED7', null);
INSERT INTO `mr_host` VALUES ('11869', 'OLD-5-2-1-2', 'OLD-5-2-1-2', '10.10.205.109', 'old0800271DDFBD', null);
INSERT INTO `mr_host` VALUES ('11871', 'OLD-5-4-2-1', 'OLD-5-4-2-1', '10.10.205.128', 'old080027911120', null);
INSERT INTO `mr_host` VALUES ('11873', 'OLD-6-3-2-2', 'OLD-6-3-2-2', '10.10.206.120', 'old080027B9FF2F', null);
INSERT INTO `mr_host` VALUES ('11875', 'OLD-6-2-1-1', 'OLD-6-2-1-1', '10.10.206.110', 'old08002781DB03', null);
INSERT INTO `mr_host` VALUES ('11877', 'OLD-6-2-2-2', 'OLD-6-2-2-2', '10.10.206.111', 'old08002701FEF1', null);
INSERT INTO `mr_host` VALUES ('11879', 'OLD-6-2-3-1', 'OLD-6-2-3-1', '10.10.206.113', 'old08002732260A', null);
INSERT INTO `mr_host` VALUES ('11881', 'OLD-4-3-1-2', 'OLD-4-3-1-2', '10.10.204.117', 'old0800272CDDF5', null);
INSERT INTO `mr_host` VALUES ('11883', 'OLD-4-3-2-1', 'OLD-4-3-2-1', '10.10.204.119', 'old080027CA2B61', null);
INSERT INTO `mr_host` VALUES ('11885', 'OLD-4-4-3-1', 'OLD-4-4-3-1', '10.10.204.130', 'old080027AFB1CE', null);
INSERT INTO `mr_host` VALUES ('11887', 'OLD-4-4-2-2', 'OLD-4-4-2-2', '10.10.204.128', 'old080027DFF54D', null);
INSERT INTO `mr_host` VALUES ('11888', 'OLD-4-4-2-1', 'OLD-4-4-2-1', '10.10.204.127', 'old080027A86865', null);
INSERT INTO `mr_host` VALUES ('11889', 'OLD-8-2-1-2', 'OLD-8-2-1-2', '10.10.208.109', 'old0800275F60CB', null);
INSERT INTO `mr_host` VALUES ('11891', 'OLD-8-4-3-2', 'OLD-8-4-3-2', '10.10.208.129', 'old0800278F99C0', null);
INSERT INTO `mr_host` VALUES ('11893', 'OLD-8-3-4-2', 'OLD-8-3-4-2', '10.10.208.124', 'old080027491B5A', null);
INSERT INTO `mr_host` VALUES ('11895', 'OLD-8-2-2-1', 'OLD-8-2-2-1', '10.10.208.112', 'old080027181BEF', null);
INSERT INTO `mr_host` VALUES ('11897', 'OLD-8-2-3-1', 'OLD-8-2-3-1', '10.10.208.113', 'old080027D44E5E', null);

-- ----------------------------
-- Table structure for mr_host_status
-- ----------------------------
DROP TABLE IF EXISTS `mr_host_status`;
CREATE TABLE `mr_host_status` (
  `pe_host_id` bigint(20) NOT NULL COMMENT '执行时主机ID',
  `pe_id` bigint(20) NOT NULL COMMENT '管道执行ID',
  `host_id` bigint(20) NOT NULL COMMENT '主机id',
  `host_status` enum('9','8','7','6','5','4','3','2','1','0') NOT NULL DEFAULT '0' COMMENT '主机状态,0未知,1正常2无反应',
  `updatetime` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP COMMENT '状态更新时间',
  PRIMARY KEY (`pe_host_id`),
  KEY `pe_id` (`pe_id`),
  KEY `host_id` (`host_id`),
  CONSTRAINT `mr_host_status_ibfk_2` FOREIGN KEY (`host_id`) REFERENCES `mr_host` (`host_id`),
  CONSTRAINT `mr_host_status_ibfk_1` FOREIGN KEY (`pe_id`) REFERENCES `mr_pl_exec` (`pe_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of mr_host_status
-- ----------------------------

-- ----------------------------
-- Table structure for mr_pipeline
-- ----------------------------
DROP TABLE IF EXISTS `mr_pipeline`;
CREATE TABLE `mr_pipeline` (
  `pl_id` bigint(20) NOT NULL COMMENT '流程ID',
  `pl_oldid` varchar(64) DEFAULT NULL COMMENT '原系统ID',
  `pl_name` varchar(50) NOT NULL COMMENT '名称',
  `pl_desc` varchar(255) NOT NULL COMMENT '说明',
  `pl_create_tm` datetime NOT NULL COMMENT '创建时间',
  `pl_modify_tm` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
  PRIMARY KEY (`pl_id`),
  UNIQUE KEY `pl_name` (`pl_name`),
  UNIQUE KEY `pl_oldid` (`pl_oldid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of mr_pipeline
-- ----------------------------
INSERT INTO `mr_pipeline` VALUES ('1', null, 'ping', 'ping所有主机', '2016-11-23 16:52:57', '2016-11-23 16:53:02');

-- ----------------------------
-- Table structure for mr_pl_edge
-- ----------------------------
DROP TABLE IF EXISTS `mr_pl_edge`;
CREATE TABLE `mr_pl_edge` (
  `edge_id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '边ID',
  `pl_id` bigint(20) NOT NULL COMMENT '所属流程ID',
  `src_id` bigint(20) NOT NULL COMMENT '引用源ID',
  `trg_id` bigint(20) NOT NULL COMMENT '引用目标ID',
  `src_type` enum('1','0') NOT NULL DEFAULT '0' COMMENT '类型：0, 脚本; 1，流程',
  `trg_type` enum('1','0') NOT NULL DEFAULT '0' COMMENT '类型：0, 脚本; 1，流程',
  `src_min_id` varchar(32) NOT NULL COMMENT '源主机MINION_ID',
  `trg_mini_id` varchar(32) NOT NULL COMMENT '目标主机MINION_ID',
  `src_timeout` int(11) DEFAULT '0' COMMENT '源节点超时时间',
  `trg_timeout` int(11) DEFAULT '0' COMMENT '目的节点超时时间',
  `err_prss_type` enum('1','0') DEFAULT '0' COMMENT '错误处理类型:0,遇到错误停止;1,遇到错误继续',
  `to_prss_type` enum('1','0') DEFAULT '0' COMMENT '超时处理类型;0,超时停止;1超时继续',
  `src_oldid` varchar(64) DEFAULT NULL COMMENT '原系统中的源ID',
  `trg_oldid` varchar(64) DEFAULT NULL COMMENT '原系统中的目标ID',
  `edge_desc` varchar(255) DEFAULT NULL COMMENT '说明',
  `create_tm` datetime NOT NULL COMMENT '创建时间',
  `modify_tm` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '修改时间',
  PRIMARY KEY (`edge_id`),
  KEY `pl_id` (`pl_id`),
  KEY `src_min_id` (`src_min_id`),
  KEY `trg_mini_id` (`trg_mini_id`),
  CONSTRAINT `mr_pl_edge_ibfk_1` FOREIGN KEY (`pl_id`) REFERENCES `mr_pipeline` (`pl_id`),
  CONSTRAINT `mr_pl_edge_ibfk_2` FOREIGN KEY (`src_min_id`) REFERENCES `mr_host` (`minion_id`),
  CONSTRAINT `mr_pl_edge_ibfk_3` FOREIGN KEY (`trg_mini_id`) REFERENCES `mr_host` (`minion_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of mr_pl_edge
-- ----------------------------
INSERT INTO `mr_pl_edge` VALUES ('1', '1', '1', '9', '0', '0', '---', '*', '0', '0', '1', '1', null, null, '开始ping', '2016-11-23 17:27:38', '2016-11-23 17:43:53');
INSERT INTO `mr_pl_edge` VALUES ('2', '1', '9', '2', '0', '0', '*', '---', '0', '0', '1', '1', null, null, '结束ping', '2016-11-23 17:28:41', '2016-11-23 17:41:13');

-- ----------------------------
-- Table structure for mr_pl_exec
-- ----------------------------
DROP TABLE IF EXISTS `mr_pl_exec`;
CREATE TABLE `mr_pl_exec` (
  `pe_id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '流程执行结果ID',
  `pl_id` bigint(20) NOT NULL COMMENT '流程ID',
  `pe_stm` datetime NOT NULL COMMENT '开始时间',
  `pe_etm` datetime DEFAULT NULL COMMENT '结束时间',
  `pe_desc` varchar(255) DEFAULT NULL COMMENT '说明',
  `pe_ret_code` int(11) NOT NULL COMMENT '返回值',
  `pe_ret_state` varchar(255) DEFAULT NULL COMMENT '执行结果或状态',
  `pe_type` enum('1','0') DEFAULT '0' COMMENT '执行类型：0,自动执行;1,手动执行',
  PRIMARY KEY (`pe_id`),
  KEY `pl_id` (`pl_id`),
  CONSTRAINT `mr_pl_exec_ibfk_1` FOREIGN KEY (`pl_id`) REFERENCES `mr_pipeline` (`pl_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of mr_pl_exec
-- ----------------------------

-- ----------------------------
-- Table structure for mr_pln_exec
-- ----------------------------
DROP TABLE IF EXISTS `mr_pln_exec`;
CREATE TABLE `mr_pln_exec` (
  `plne_id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '节点执行结果ID',
  `pe_id` bigint(20) NOT NULL COMMENT '流程执行ID',
  `edge_id` bigint(20) NOT NULL COMMENT '节点ID',
  `salt_jid` varchar(32) DEFAULT NULL COMMENT 'Salt Job ID',
  `pid` int(11) DEFAULT NULL COMMENT '进程ID',
  `ret_code` int(11) DEFAULT NULL COMMENT '执行结果代码',
  `pln_stm` timestamp NULL DEFAULT NULL COMMENT '开始时间',
  `pln_etm` timestamp NULL DEFAULT NULL COMMENT '结束时间',
  `ret_stderr` varchar(255) DEFAULT NULL COMMENT '标准错误输出',
  `ret_stdout` varchar(255) DEFAULT NULL COMMENT '标准输出',
  `minion_id` varchar(32) DEFAULT NULL COMMENT '主机MinionID',
  `arg` varchar(255) DEFAULT NULL COMMENT '运行参数',
  `fun` varchar(32) DEFAULT NULL COMMENT '运行函数',
  PRIMARY KEY (`plne_id`),
  KEY `mr_pln_exec_ibfk_3` (`edge_id`),
  KEY `mr_pln_exec_ibfk_4` (`pe_id`) USING BTREE,
  CONSTRAINT `mr_pln_exec_ibfk_4` FOREIGN KEY (`pe_id`) REFERENCES `mr_pl_exec` (`pe_id`),
  CONSTRAINT `mr_pln_exec_ibfk_3` FOREIGN KEY (`edge_id`) REFERENCES `mr_pl_edge` (`edge_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

-- ----------------------------
-- Records of mr_pln_exec
-- ----------------------------

-- ----------------------------
-- Table structure for mr_script
-- ----------------------------
DROP TABLE IF EXISTS `mr_script`;
CREATE TABLE `mr_script` (
  `scpt_id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '脚本ID',
  `scpt_name` varchar(50) NOT NULL COMMENT '脚本名称',
  `host_id` bigint(20) DEFAULT NULL COMMENT '脚本对应的主机ID',
  `scpt_script` text NOT NULL COMMENT '脚本',
  `scpt_type` enum('1','0') NOT NULL DEFAULT '0' COMMENT '0,调用执行器;1,salt命令',
  `scpt_timeout` int(11) NOT NULL DEFAULT '300' COMMENT '执行超时时间',
  `scpt_desc` varchar(255) NOT NULL COMMENT '说明',
  `scpt_create_tm` datetime NOT NULL COMMENT '创建时间',
  `scpt_modify_tm` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '修改时间',
  PRIMARY KEY (`scpt_id`),
  UNIQUE KEY `script` (`scpt_name`,`host_id`),
  FULLTEXT KEY `scpt_name` (`scpt_name`),
  FULLTEXT KEY `scpt_script` (`scpt_script`)
) ENGINE=MyISAM AUTO_INCREMENT=10 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of mr_script
-- ----------------------------
INSERT INTO `mr_script` VALUES ('1', '开始', '0', '\'\'', '0', '300', '\'\'', '2016-11-22 13:30:14', '2016-11-23 15:46:50');
INSERT INTO `mr_script` VALUES ('2', '结束', '0', '\'\'', '0', '300', '\'\'', '2016-11-22 13:33:08', '2016-11-23 15:46:55');
INSERT INTO `mr_script` VALUES ('3', 'halt', '0', 'system.halt', '1', '300', 'Halt a running system', '2016-11-23 15:49:20', '2016-11-23 15:49:23');
INSERT INTO `mr_script` VALUES ('4', 'init', '0', 'system.init %d', '1', '300', 'Change the system runlevel on sysV compatible systems, salt \'*\' system.init 3', '2016-11-23 15:50:22', '2016-11-23 16:00:55');
INSERT INTO `mr_script` VALUES ('5', '关闭电源', '0', 'system.poweroff', '1', '300', 'Poweroff a running system', '2016-11-23 15:52:27', '2016-11-23 15:52:30');
INSERT INTO `mr_script` VALUES ('6', '重启', '0', 'system.reboot %d', '1', '300', 'The wait time in minutes before the system will be rebooted.', '2016-11-23 16:00:22', '2016-11-23 16:01:55');
INSERT INTO `mr_script` VALUES ('7', '关机', '0', 'system.shutdown %d', '1', '300', 'The wait time in minutes before the system will be shutdown.', '2016-11-23 16:04:21', '2016-11-23 16:04:25');
INSERT INTO `mr_script` VALUES ('8', '手工确认', '0', '\'\'', '0', '300', '需要手工确认的节点', '2016-11-23 16:45:46', '2016-11-23 16:45:49');
INSERT INTO `mr_script` VALUES ('9', 'ping', '0', 'test.ping', '1', '300', 'ping所有主机', '2016-11-23 16:53:53', '2016-11-23 16:53:57');

-- ----------------------------
-- View structure for v_pe_host_status
-- ----------------------------
DROP VIEW IF EXISTS `v_pe_host_status`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`%` SQL SECURITY DEFINER VIEW `v_pe_host_status` AS select `mr_host_status`.`pe_host_id` AS `pe_host_id`,`mr_host_status`.`pe_id` AS `pe_id`,`mr_host_status`.`host_id` AS `host_id`,`mr_host_status`.`host_status` AS `host_status`,`mr_host_status`.`updatetime` AS `updatetime`,`mr_host`.`host` AS `host`,`mr_host`.`name` AS `name`,`mr_host`.`ip` AS `ip`,`mr_host`.`minion_id` AS `minion_id`,`mr_pipeline`.`pl_name` AS `pl_name`,`mr_pl_exec`.`pe_stm` AS `pe_stm`,`mr_pl_exec`.`pe_etm` AS `pe_etm`,`mr_pl_exec`.`pe_desc` AS `pe_desc`,`mr_pl_exec`.`pe_ret_code` AS `pe_ret_code`,`mr_pl_exec`.`pe_ret_state` AS `pe_ret_state`,`mr_pl_exec`.`pe_type` AS `pe_type`,`mr_pl_exec`.`pl_id` AS `pl_id`,`mr_pipeline`.`pl_desc` AS `pl_desc` from (((`mr_host_status` join `mr_host` on((`mr_host_status`.`host_id` = `mr_host`.`host_id`))) join `mr_pl_exec` on((`mr_host_status`.`pe_id` = `mr_pl_exec`.`pe_id`))) join `mr_pipeline` on((`mr_pl_exec`.`pl_id` = `mr_pipeline`.`pl_id`))) ;

-- ----------------------------
-- View structure for v_pl_exec
-- ----------------------------
DROP VIEW IF EXISTS `v_pl_exec`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`%` SQL SECURITY DEFINER VIEW `v_pl_exec` AS select `mr_pl_exec`.`pe_id` AS `pe_id`,`mr_pl_exec`.`pl_id` AS `pl_id`,`mr_pl_exec`.`pe_stm` AS `pe_stm`,`mr_pl_exec`.`pe_etm` AS `pe_etm`,`mr_pl_exec`.`pe_desc` AS `pe_desc`,`mr_pl_exec`.`pe_ret_code` AS `pe_ret_code`,`mr_pl_exec`.`pe_ret_state` AS `pe_ret_state`,`mr_pl_exec`.`pe_type` AS `pe_type`,`mr_pipeline`.`pl_oldid` AS `pl_oldid`,`mr_pipeline`.`pl_name` AS `pl_name`,`mr_pipeline`.`pl_desc` AS `pl_desc`,`mr_pipeline`.`pl_create_tm` AS `pl_create_tm`,`mr_pipeline`.`pl_modify_tm` AS `pl_modify_tm` from (`mr_pl_exec` join `mr_pipeline` on((`mr_pl_exec`.`pl_id` = `mr_pipeline`.`pl_id`))) group by `mr_pl_exec`.`pe_id`,`mr_pl_exec`.`pl_id` order by `mr_pl_exec`.`pe_id`,`mr_pl_exec`.`pl_id` ;

-- ----------------------------
-- View structure for v_ple_exec
-- ----------------------------
DROP VIEW IF EXISTS `v_ple_exec`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`%` SQL SECURITY DEFINER VIEW `v_ple_exec` AS select `mr_pln_exec`.`plne_id` AS `plne_id`,`mr_pln_exec`.`pe_id` AS `pe_id`,`mr_pln_exec`.`edge_id` AS `edge_id`,`mr_pln_exec`.`salt_jid` AS `salt_jid`,`mr_pln_exec`.`pid` AS `pid`,`mr_pln_exec`.`ret_code` AS `ret_code`,`mr_pln_exec`.`pln_stm` AS `pln_stm`,`mr_pln_exec`.`pln_etm` AS `pln_etm`,`mr_pln_exec`.`ret_stderr` AS `ret_stderr`,`mr_pln_exec`.`ret_stdout` AS `ret_stdout`,`mr_pln_exec`.`minion_id` AS `minion_id`,`mr_pln_exec`.`arg` AS `arg`,`mr_pln_exec`.`fun` AS `fun`,`mr_pl_edge`.`src_id` AS `src_id`,`mr_pl_edge`.`src_type` AS `src_type`,`mr_pl_edge`.`trg_type` AS `trg_type`,`mr_pl_edge`.`src_min_id` AS `src_min_id`,`mr_pl_edge`.`trg_mini_id` AS `trg_mini_id`,`mr_pl_edge`.`err_prss_type` AS `err_prss_type`,`mr_pl_edge`.`to_prss_type` AS `to_prss_type`,`mr_pl_edge`.`src_oldid` AS `src_oldid`,`mr_pl_edge`.`trg_oldid` AS `trg_oldid`,`mr_pl_edge`.`edge_desc` AS `edge_desc`,`mr_pl_edge`.`create_tm` AS `create_tm`,`mr_pl_edge`.`modify_tm` AS `modify_tm`,`mr_pl_exec`.`pl_id` AS `pl_id`,`mr_pl_exec`.`pe_stm` AS `pe_stm`,`mr_pl_exec`.`pe_etm` AS `pe_etm`,`mr_pl_exec`.`pe_desc` AS `pe_desc`,`mr_pl_exec`.`pe_ret_code` AS `pe_ret_code`,`mr_pl_exec`.`pe_ret_state` AS `pe_ret_state`,`mr_pl_exec`.`pe_type` AS `pe_type`,`mr_pipeline`.`pl_oldid` AS `pl_oldid`,`mr_pipeline`.`pl_name` AS `pl_name`,`mr_pipeline`.`pl_desc` AS `pl_desc`,`mr_pipeline`.`pl_create_tm` AS `pl_create_tm`,`mr_pipeline`.`pl_modify_tm` AS `pl_modify_tm`,`mr_pl_edge`.`src_timeout` AS `src_timeout`,`mr_pl_edge`.`trg_timeout` AS `trg_timeout` from (((`mr_pln_exec` join `mr_pl_edge` on((`mr_pln_exec`.`edge_id` = `mr_pl_edge`.`edge_id`))) join `mr_pl_exec` on((`mr_pln_exec`.`pe_id` = `mr_pl_exec`.`pe_id`))) join `mr_pipeline` on(((`mr_pl_edge`.`pl_id` = `mr_pipeline`.`pl_id`) and (`mr_pl_exec`.`pl_id` = `mr_pipeline`.`pl_id`)))) group by `mr_pln_exec`.`pe_id`,`mr_pl_edge`.`pl_id`,`mr_pln_exec`.`edge_id` ;

-- ----------------------------
-- View structure for v_ple_s2s
-- ----------------------------
DROP VIEW IF EXISTS `v_ple_s2s`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`%` SQL SECURITY DEFINER VIEW `v_ple_s2s` AS select `edge`.`edge_id` AS `edge_id`,`edge`.`pl_id` AS `pl_id`,`pipeline`.`pl_name` AS `pl_name`,`pipeline`.`pl_desc` AS `pl_desc`,`edge`.`src_id` AS `src_id`,`edge`.`src_type` AS `src_type`,`src_scpt`.`scpt_name` AS `src_scpt_name`,`src_scpt`.`scpt_script` AS `src_scpt`,`edge`.`trg_id` AS `trg_id`,`edge`.`trg_type` AS `trg_type`,`trg_scpt`.`scpt_name` AS `trg_scpt_name`,`trg_scpt`.`scpt_script` AS `trg_sctp`,`edge`.`src_min_id` AS `src_min_id`,`src_host`.`host` AS `src_host`,`src_host`.`ip` AS `src_host_ip`,`edge`.`trg_mini_id` AS `trg_mini_id`,`trg_host`.`host` AS `trg_host`,`trg_host`.`ip` AS `trg_host_ip`,`edge`.`src_timeout` AS `src_timeout`,`edge`.`trg_timeout` AS `trg_timeout`,`edge`.`err_prss_type` AS `err_prss_type`,`edge`.`to_prss_type` AS `to_prss_type`,`edge`.`src_oldid` AS `src_oldid`,`edge`.`trg_oldid` AS `trg_oldid`,`edge`.`edge_desc` AS `edge_desc`,`edge`.`create_tm` AS `create_tm`,`edge`.`modify_tm` AS `modify_tm` from (((((`mr_pl_edge` `edge` join `mr_pipeline` `pipeline` on((`edge`.`pl_id` = `pipeline`.`pl_id`))) join `mr_host` `trg_host` on((`edge`.`trg_mini_id` = `trg_host`.`minion_id`))) join `mr_host` `src_host` on((`edge`.`src_min_id` = `src_host`.`minion_id`))) join `mr_script` `src_scpt` on(((`edge`.`src_id` = `src_scpt`.`scpt_id`) and (`edge`.`src_type` = '0')))) join `mr_script` `trg_scpt` on(((`edge`.`trg_id` = `trg_scpt`.`scpt_id`) and (`edge`.`trg_type` = '0')))) ;
