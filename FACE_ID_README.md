# 📸 Face ID - Guide d'utilisation

## ✅ Ce qui a été implémenté

### 1. **Backend (Classe `employee`)**
- ✅ `saveFaceImage(email, image)` - Enregistrer une photo de visage
- ✅ `getFaceImage(email)` - Récupérer la photo d'un employé
- ✅ `authenticateByFace(image)` - Authentifier par reconnaissance faciale

### 2. **Frontend (LoginDialog)**
- ✅ Bouton "📸 Face ID" dans l'écran de connexion
- ✅ Interface de capture vidéo en temps réel
- ✅ Authentification automatique après capture

### 3. **Frontend (Gemploye)**
- ✅ Fonction `captureFaceImage()` pour capturer le visage
- ✅ Sauvegarde automatique lors de l'ajout d'un employé
- ✅ Sauvegarde automatique lors de la modification d'un employé

### 4. **Configuration**
- ✅ Modules Qt `multimedia` et `multimediawidgets` ajoutés au projet
- ✅ Colonne `FACE_IMAGE` BLOB dans la base de données

---

## 🚀 Installation

### Étape 1: Base de données
Exécutez le script SQL :
```bash
sqlplus votre_user/votre_password@votre_db < FACE_ID_SETUP.sql
```

OU ouvrez SQL Developer et exécutez :
```sql
ALTER TABLE EMPLOYE ADD FACE_IMAGE BLOB;
```

### Étape 2: Compiler le projet
1. Ouvrez le projet dans Qt Creator
2. **Build → Clean All**
3. **Build → Rebuild All**

### Étape 3: Tester
Lancez l'application depuis Qt Creator

---

## 📖 Comment utiliser

### A. **Enregistrer un visage (lors de l'ajout d'un employé)**

**IMPORTANT :** Tu dois d'abord ajouter un bouton "Capturer visage" dans l'interface `gemploye.ui`.

#### Option 1 : Avec Qt Designer
1. Ouvre `gemploye.ui` dans Qt Designer
2. Ajoute un `QPushButton` dans le formulaire d'ajout d'employé
3. Nomme-le : `btnCaptureFace`
4. Texte du bouton : `📸 Capturer visage`
5. Sauvegarde

#### Option 2 : Je peux le faire pour toi
Si tu veux, réponds-moi et je modifierai le fichier .ui directement.

**Utilisation :**
1. Remplis les champs de l'employé (CIN, nom, email, etc.)
2. Clique sur **"📸 Capturer visage"**
3. La caméra s'ouvre
4. Positionne ton visage devant la caméra
5. Clique sur **"📸 Capturer"**
6. Clique sur **"ajouter"** pour sauvegarder l'employé avec sa photo

### B. **Se connecter avec Face ID**

1. Lance l'application
2. Dans l'écran de connexion, clique sur **"📸 Face ID"**
3. La caméra s'ouvre automatiquement
4. Positionne ton visage devant la caméra
5. Clique sur **"Capturer et Authentifier"**
6. Si ton visage est reconnu, tu es connecté automatiquement

---

## ⚠️ Limitations actuelles (Version Prototype)

Cette version est un **prototype** qui :
- ✅ Capture et stocke les photos de visages
- ✅ Permet l'authentification par bouton Face ID
- ⚠️ **N'utilise PAS encore de vraie reconnaissance faciale**
- ⚠️ Pour l'instant, authentifie le premier employé qui a une photo

### 🔄 Pour une vraie reconnaissance faciale

Pour upgrader vers une vraie reconnaissance faciale, il faudra :
1. **Installer OpenCV** (bibliothèque de vision par ordinateur)
2. **Installer dlib** ou **face_recognition** (reconnaissance faciale)
3. **Modifier `authenticateByFace()`** pour comparer les visages pixel par pixel
4. **Ajouter la détection de visage** pour extraire uniquement le visage de la photo

---

## 🎯 Prochaines étapes

Si tu veux améliorer le système :

### Option A : Ajouter le bouton dans gemploye.ui
Dis-moi et je le fais pour toi

### Option B : Upgrader vers vraie reconnaissance faciale
Installer OpenCV et implémenter la comparaison réelle

### Option C : Tester la version actuelle
Compile et teste pour voir si ça marche

---

## 🐛 Dépannage

### Erreur: "Aucune caméra détectée"
- Vérifie que ta webcam est branchée et activée
- Sur Windows, vérifie les permissions de la caméra dans Paramètres → Confidentialité

### Erreur: "FACE_IMAGE column does not exist"
- Execute le script SQL : `ALTER TABLE EMPLOYE ADD FACE_IMAGE BLOB;`

### Le bouton "Capturer visage" n'existe pas
- Tu dois l'ajouter manuellement dans `gemploye.ui` (voir section "Comment utiliser")

---

## 💡 Notes importantes

- La photo est stockée en **BLOB** dans Oracle (taille max ~128MB par défaut)
- Format d'image : **PNG** (meilleure qualité que JPEG)
- Chaque employé ne peut avoir **qu'une seule photo**
- La photo est **remplacée** si tu captures à nouveau

---

**Bon courage ! 🚀**

Si tu as des questions, n'hésite pas !
