import time
import sys
from arduino import app_utils

print("[INFO] Client Bridge initialisé.")

input_text = "Id:2" # 'move:100,20'

i = 1

while True:
    i += 1;
    if i > 36:
        i = 1

    if "Id" in input_text:
        input_text = f""
    else:
        input_text = f"Id:{i}"
        
    time.sleep(5)
    
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
