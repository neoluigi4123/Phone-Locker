import face_recognition as fr
import cv2
import numpy as np

face_bank = [None] * 36  # Initialisation de la banque des têtes (36 emplacements)

def encode_faces(image_path):
    """
    Cette fonction donne une liste des visages sur une image
    """
    face_locations = fr.face_locations(image_path)
    face_encodings = fr.face_encodings(image_path, face_locations)

    return face_locations, face_encodings

def get_stored_faces():
    """
    Cette fonction donne la liste des visages stockés et leurs index
    """
    indices = []
    encodings = []
    for i, face in enumerate(face_bank):
        if face is not None:
            indices.append(i)
            encodings.append(face)
    return indices, encodings

def find_smallest_available():
    """
    Cette fonction donne le plus petit index disponible ou -1 si plein
    """
    for i in range(36):
        if face_bank[i] is None:
            return i
    return -1

while True:
    # Demande l'image
    image = input("Image a utilisé: ")
    if image == "list":
        indices, encodings = get_stored_faces()
        print(f"Visage Sauvegardé: {len(indices)} visage{"s" if len(indices)>1 else ""}")
        for i, enc in zip(indices, encodings):
            print(f"  ID {i+1}: {enc[:5]}")  # Affiche l'id et un échantillon des features
        continue

    image_loaded = fr.load_image_file(image)

    # Process l'image
    small_frame = cv2.resize(image_loaded, (0, 0), fx=0.25, fy=0.25)  # Reduit la résolution de l'image

    # Encode l'image
    face_locations, encoded_faces = encode_faces(small_frame)

    if len(encoded_faces) == 0:  # Aucun visage détecté
        print("no face detected")
        continue

    # Garde uniquement le plus grand visage (le plus proche)
    biggest_index = 0
    biggest_area = 0
    for i, (top, right, bottom, left) in enumerate(face_locations):
        area = (bottom - top) * (right - left)  # Calcule la surface du visage
        if area > biggest_area:
            biggest_area = area
            biggest_index = i

    face = encoded_faces[biggest_index]  # Le visage le plus grand/proche

    indices, encodings = get_stored_faces()

    if len(encodings) == 0:
        slot = find_smallest_available()
        face_bank[slot] = face  # Ajout du visage dans la banque
        print(f"new face detected (ID {slot+1})")
    else:
        matches = fr.compare_faces(encodings, face)  # Compare les visages
        face_distances = fr.face_distance(encodings, face)  # Regarde la ressemblance des visages

        best_match_index = np.argmin(face_distances)  # Regarde les visages qui se ressemble

        if matches[best_match_index]:  # Le visage est reconnu
            real_index = indices[best_match_index]  # Retrouve l'index réel dans la banque
            face_bank[real_index] = None  # Retire le visage de la banque
            print(f"face found (ID {real_index+1})")

        else:  # Le visage n'est pas reconnu
            slot = find_smallest_available()
            if slot == -1:  # Plus de place dans la banque
                print("error: la banque est pleine (36/36)")
            else:
                face_bank[slot] = face  # Ajout du visage dans la banque
                print(f"new face detected (ID {slot+1})")
