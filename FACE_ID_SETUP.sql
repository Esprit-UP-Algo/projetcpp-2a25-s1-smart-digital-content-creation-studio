-- =====================================================
-- Script SQL pour activer le Face ID
-- =====================================================
-- Exécutez ces commandes dans votre base de données Oracle

-- 1. Ajouter la colonne FACE_IMAGE pour stocker les photos
ALTER TABLE EMPLOYE ADD FACE_IMAGE BLOB;

-- 2. Vérifier que la colonne a été ajoutée
DESC EMPLOYE;

-- 3. (Optionnel) Si vous voulez voir quels employés ont une photo
SELECT EMAIL, 
       CASE WHEN FACE_IMAGE IS NULL THEN 'Pas de photo' ELSE 'Photo enregistrée' END AS STATUT_FACE_ID
FROM EMPLOYE;

-- =====================================================
-- Note: La colonne FACE_IMAGE peut être NULL
-- Elle sera remplie automatiquement lors de la capture
-- =====================================================
