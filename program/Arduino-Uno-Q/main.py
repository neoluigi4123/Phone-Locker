import time
import sys
from arduino import app_utils

import face_recognition as fr
import cv2
import numpy as np
import threading

face_bank = [None] * 36  # Initialisation de la banque des têtes (36 emplacements)

class VideoCaptureLatest:
    def __init__(self, source='/dev/video0'):
        self.cap = cv2.VideoCapture(source)
        self.lock = threading.Lock()
        self.ret, self.frame = self.cap.read()
        self.running = True
        
        self.thread = threading.Thread(target=self._update, daemon=True)
        self.thread.start()

    def _update(self):
        # Garde le buffer de la camera a jour avec la dernière frame
        while self.running:
            ret, frame = self.cap.read()
            with self.lock:
                self.ret = ret
                self.frame = frame

    def read(self):
        # Donne une copie de la dernière frame de la camera
        with self.lock:
            return self.ret, self.frame.copy() if self.ret else None

    def release(self):
        self.running = False
        self.thread.join()
        self.cap.release()

# Replace cv2.VideoCapture with our new threading class
video_capture = VideoCaptureLatest('/dev/video2') #http://192.168.43.1:4747

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
    encodings =[]
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

def send_command(query):
    """
    Cette fonction envoie une commande a la carte arduino
    """
    try:
        response = app_utils.Bridge.call("traiter_commande", query)
        if response is not None:
            print(f"[ARDUINO] {response}")
        else:
            print("[INFO] Aucune valeur retournée.")
                
    except Exception as e:
        print(f"[ERREUR Bridge] {e}")

while True:
    input_text = ""
    try:
        time.sleep(2)
        
        # Process l'image
        print("[*]  capturing camera")    
        ret, frame = video_capture.read()
    
        if not ret or frame is None:
            print("[x]  error: impossible de lire la caméra")
            continue

        small_frame = cv2.resize(frame, (0, 0), fx=0.25, fy=0.25) # Reduit la résolution de l'image
        small_frame = np.ascontiguousarray(small_frame[:, :, ::-1]) # Convertie le BGR en RGB pour Facial Recognition

        # Encode l'image
        face_locations, encoded_faces = encode_faces(small_frame)

        if len(encoded_faces) == 0:  # Aucun visage détecté
            print("[ ]  no face detected")
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
            print(f"[+]  new face detected (ID {slot+1})")
            input_text = f"Id:{slot+1}"
        else:
            matches = fr.compare_faces(encodings, face)  # Compare les visages
            face_distances = fr.face_distance(encodings, face)  # Regarde la ressemblance des visages

            best_match_index = np.argmin(face_distances)  # Regarde les visages qui se ressemblent

            if matches[best_match_index]:  # Le visage est reconnu
                real_index = indices[best_match_index]  # Retrouve l'index réel dans la banque
                face_bank[real_index] = None  # Retire le visage de la banque
                print(f"[-]  face found (ID {real_index+1})")
                input_text = f"Id:{real_index+1}"

            else:  # Le visage n'est pas reconnu
                slot = find_smallest_available()
                if slot == -1:  # Plus de place dans la banque
                    print("[x]  error: la banque est pleine (36/36)")
                else:
                    face_bank[slot] = face  # Ajout du visage dans la banque
                    print(f"[+]  new face detected (ID {slot+1})")
                    input_text = f"Id:{slot+1}"
    
    except Exception as e:
        print(f"[ERREUR] {e}")
        continue

    try:
        if input_text:
            send_command(input_text)
    except Exception as e:
        print(f"[ERREUR] {e}")
    
    # Check if face was detected and sent to Arduino, wait unill the face goes away to send the `fermer` command
    if "Id" in input_text:
        while True:
            ret, frame = video_capture.read()
            if not ret or frame is None:
                print("[x]  error: impossible de lire la caméra")
                break

            small_frame = cv2.resize(frame, (0, 0), fx=0.25, fy=0.25)
            small_frame = np.ascontiguousarray(small_frame[:, :, ::-1])

            face_locations, encoded_faces = encode_faces(small_frame)

            # No face at all
            if len(encoded_faces) == 0:  # Aucun visage détecté
                print("[ ]  no face detected, sending 'fermer' command")
                send_command("Fermer")
                break

            # Face small enough
            biggest_area = 0
            for (top, right, bottom, left) in face_locations:
                area = (bottom - top) * (right - left)
                if area > biggest_area:
                    biggest_area = area
            if biggest_area < 1500:  # Seuil de surface pour considerer que le visage est "loin"
                print("[ ]  face too small, sending 'fermer' command")
                send_command("Fermer")
                break

            time.sleep(1)
