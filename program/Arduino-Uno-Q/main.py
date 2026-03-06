import time
import sys
from arduino import app_utils

print("[INFO] Client Bridge initialisé.")

input_text = "move:100,20"

while True:
    if input_text == "move:0,20":
        input_text = "move:100,10"
    else:
        input_text = "move:0,20"
        
    time.sleep(2)
    
    try:
        query = input_text
        if query.lower() in ["exit", "quit"]:
            break
        
        try:
            response = app_utils.Bridge.call("traiter_commande", query)
            if response is not None:
                print(f"[ARDUINO] {response}")
            else:
                print("[INFO] Aucune valeur retournée.")
                
        except Exception as e:
            print(f"[ERREUR Bridge] {e}")

    except EOFError:
        break
    except KeyboardInterrupt:
        print("\nArrêt.")
        break
