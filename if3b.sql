-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Hôte : 127.0.0.1
-- Généré le : lun. 16 déc. 2024 à 10:25
-- Version du serveur : 10.4.32-MariaDB
-- Version de PHP : 8.1.25

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Base de données : `if3b`
--

-- --------------------------------------------------------

--
-- Structure de la table `capteur_humidite`
--

CREATE TABLE `capteur_humidite` (
  `value` int(11) NOT NULL,
  `timestamp` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Déchargement des données de la table `capteur_humidite`
--

INSERT INTO `capteur_humidite` (`value`, `timestamp`) VALUES
(10, '2024-12-15 21:50:35'),
(20, '2024-12-15 21:50:38'),
(15, '2024-12-15 21:50:41'),
(16, '2024-12-15 21:50:43'),
(20, '2024-12-15 21:50:45'),
(24, '2024-12-15 21:50:47'),
(26, '2024-12-15 21:50:51'),
(27, '2024-12-15 21:50:54'),
(25, '2024-12-15 21:50:56'),
(29, '2024-12-15 21:50:59'),
(30, '2024-12-15 21:51:01'),
(35, '2024-12-15 21:51:03'),
(40, '2024-12-15 21:51:06'),
(45, '2024-12-15 21:51:08'),
(40, '2024-12-15 21:51:12'),
(20, '2024-12-15 21:51:18');

-- --------------------------------------------------------

--
-- Structure de la table `capteur_temperature`
--

CREATE TABLE `capteur_temperature` (
  `value` int(11) NOT NULL,
  `timestamp` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Déchargement des données de la table `capteur_temperature`
--

INSERT INTO `capteur_temperature` (`value`, `timestamp`) VALUES
(10, '2024-12-15 21:51:42'),
(30, '2024-12-15 21:51:44'),
(25, '2024-12-15 21:51:46'),
(24, '2024-12-15 21:51:48'),
(26, '2024-12-15 21:51:51'),
(35, '2024-12-15 21:51:54'),
(40, '2024-12-15 21:51:56'),
(30, '2024-12-15 21:51:59'),
(26, '2024-12-15 21:52:02'),
(22, '2024-12-15 21:52:05'),
(19, '2024-12-15 21:52:09'),
(20, '2024-12-15 21:52:57');

-- --------------------------------------------------------

--
-- Structure de la table `lightsensor`
--

CREATE TABLE `lightsensor` (
  `light_level` int(11) NOT NULL,
  `timestamp` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Déchargement des données de la table `lightsensor`
--

INSERT INTO `lightsensor` (`light_level`, `timestamp`) VALUES
(30, '2024-12-15 21:25:33'),
(33, '2024-12-15 21:25:35'),
(34, '2024-12-15 21:25:38'),
(35, '2024-12-15 21:25:40'),
(37, '2024-12-15 21:25:42'),
(40, '2024-12-15 21:25:44'),
(42, '2024-12-15 21:25:47');
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
