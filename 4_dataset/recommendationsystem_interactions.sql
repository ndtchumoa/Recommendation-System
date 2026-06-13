-- MySQL dump 10.13  Distrib 8.0.46, for Win64 (x86_64)
--
-- Host: localhost    Database: recommendationsystem
-- ------------------------------------------------------
-- Server version	8.0.46

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `interactions`
--

DROP TABLE IF EXISTS `interactions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `interactions` (
  `id` int NOT NULL AUTO_INCREMENT,
  `user_id` varchar(20) NOT NULL,
  `item_id` varchar(20) NOT NULL,
  `click` int NOT NULL DEFAULT '0',
  `add_cart` int NOT NULL DEFAULT '0',
  `purchase` int NOT NULL DEFAULT '0',
  `rating` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`),
  KEY `item_id` (`item_id`),
  CONSTRAINT `interactions_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `users` (`user_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `interactions_ibfk_2` FOREIGN KEY (`item_id`) REFERENCES `items` (`item_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=60 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `interactions`
--

LOCK TABLES `interactions` WRITE;
/*!40000 ALTER TABLE `interactions` DISABLE KEYS */;
INSERT INTO `interactions` VALUES (1,'U001','I001',3,1,1,5),(2,'U001','I002',5,2,1,4),(3,'U001','I003',2,1,0,0),(4,'U001','I006',4,0,1,5),(5,'U001','I007',3,1,1,4),(6,'U001','I012',2,1,1,4),(7,'U002','I001',2,0,1,5),(8,'U002','I004',6,2,1,5),(9,'U002','I005',1,1,0,0),(10,'U002','I008',5,1,1,5),(11,'U002','I009',3,0,0,0),(12,'U002','I013',4,2,1,4),(13,'U003','I002',7,3,1,5),(14,'U003','I003',4,2,1,5),(15,'U003','I004',3,1,1,4),(16,'U003','I010',2,1,0,0),(17,'U003','I014',5,2,1,5),(18,'U003','I015',6,1,1,4),(19,'U004','I001',1,0,1,4),(20,'U004','I005',3,1,1,5),(21,'U004','I006',2,0,0,0),(22,'U004','I011',4,2,1,4),(23,'U004','I012',3,1,1,5),(24,'U004','I013',2,1,1,3),(25,'U005','I002',4,2,1,4),(26,'U005','I007',3,1,1,5),(27,'U005','I008',5,2,1,5),(28,'U005','I009',2,1,0,0),(29,'U005','I015',4,1,1,5),(30,'U006','I003',3,1,1,4),(31,'U006','I004',2,0,0,0),(32,'U006','I006',6,2,1,5),(33,'U006','I007',5,2,1,5),(34,'U006','I008',4,1,1,4),(35,'U006','I010',3,1,1,3),(36,'U007','I001',5,2,1,5),(37,'U007','I002',3,1,1,4),(38,'U007','I005',4,2,1,5),(39,'U007','I011',2,1,0,0),(40,'U007','I013',6,2,1,5),(41,'U007','I014',4,1,1,4),(42,'U008','I003',2,1,0,0),(43,'U008','I004',5,2,1,5),(44,'U008','I009',3,0,1,3),(45,'U008','I010',4,1,1,4),(46,'U008','I012',6,2,1,5),(47,'U008','I015',3,1,1,4),(48,'U009','I001',4,1,1,4),(49,'U009','I002',2,0,0,0),(50,'U009','I006',5,2,1,5),(51,'U009','I007',4,2,1,5),(52,'U009','I008',3,1,1,4),(53,'U009','I011',2,1,1,3),(54,'U010','I003',6,2,1,5),(55,'U010','I004',4,1,1,4),(56,'U010','I005',3,1,0,0),(57,'U010','I013',5,2,1,4),(58,'U010','I014',4,2,1,5),(59,'U010','I015',2,0,1,3);
/*!40000 ALTER TABLE `interactions` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2026-06-14  0:52:37
