# CAHIER DES CHARGES PHONE LOCKER

## 1. Présentation générale du projet
   
   ### 1.1 Contexte
   
   Dans le cadre du projet de Terminale STI2D spécialité SIN, nous devons concevoir un système électronique automatisé répondant à un besoin réel.
   
   ### 1.2 Besoin à satisfaire
   
   Limiter l’usage du téléphone portable dans un lieu donné (salle de classe, réunion, etc.) en proposant une boîte sécurisée, intelligente et automatisée permettant :

   - Déposer les téléphones
   
   - Les récupérer avec une reconnaissance faciale
      
## 2. Fonction globale du système
   
   Stocker jusqu’à 36 téléphones dans un système sécurisé et automatisé, en autorisant l’accès uniquement aux utilisateurs reconnus.

## 3. Service Functions (Fonctions de service)

| N° | Fonction de service | Description |
|:--:|:---|:---|
| **FS1** | Identifier l’utilisateur | Le système doit reconnaître le visage de l’utilisateur via la caméra |
| **FS2** | Autoriser ou refuser l’accès | En fonction du résultat de la reconnaissance faciale |
| **FS3** | Ranger les téléphones | Chaque téléphone est placé dans son compartiment dédié |
| **FS4** | Accéder aux téléphones | Une porte motorisée permet l’accès au compartiment en descendant vers le dessous du chassis |
| **FS5** | Informer l’utilisateur | Affichage d’informations sur un écran LCD et/ou LED |
| **FS6** | Permettre une commande manuelle en cas de problème | Trois boutons accessibles via la trappe à aimant à l'arrière |

## 4. Technical Functions (Fonctions techniques)

| Fonction technique | Solution retenue |
|:---|:---|
| Traitement des données | Carte Arduino UNO Q |
| Reconnaissance faciale | Caméra USB 420p et 30fps |
| Rotation du plateau | Moteur électrique avec encodeur |
| Ouverture de la porte | Moteur électrique |
| Interface utilisateur | Écran LCD + boutons poussoirs |
| Signalisation | buzzer |
| Alimentation | Piles |
| ( Eclairer l'utilisateur ) | ( LED ) |

## 5. Description des composants
   
   ### 5.1 Carte Arduino
   
   - Carte :  Arduino UNO Q
   
   - Utilité :
   
        Gestion des moteurs
   
        Communication avec la caméra
   
        Traitement de la reconnaissance faciale (Python)
   
   ### 5.2 Caméra USB
   
   - Specs : 420p, 30 fps
   
   - Utilité :

        Détection et analyse du visage
   
        Faible résolution pour un traitement rapide et fluide
   
### 5.3 Interface utilisateur

 - Écran LCD : Affichage d’instructions, état du système ?
 - Boutons (x3) 
 - LED ?
 - Buzzer ?
   
## 6. Critères de performance
| Critère |	Valeur attendue |
|:---|:---|
|Nombre de téléphones |	36 |
| Temps de reconnaissance | ? |
| Précision reconnaissance | Suffisante pour distinguer les utilisateurs |
| Temps d’ouverture | ? |

