"""
Script python
"""

import time
import serial # permet la communication entre appareil usb

class arduino:
    def __init__(self, device: str = '/dev/ttyUSB0', baudrate: int = 9600, timeout: float = 1.0) -> None:
        """
        Initialiser la communication entre le programme python et le cœur arduino.
        """

	try:
            self.serial = serial.Serial(device, baudrate, timeout)
	    time.sleep(timeout + 1)
	    self.serial.reset_input_buffer()

	except Exception as e:
	    return(f"[ERREUR] > __init__: {e}")


    def get(self) -> str | None:
	"""
	Lire les messages envoyé par la carte Arduino.
	"""

	try:
	    if self.serial.in_waiting > 0:

		line = self.serial.readline().decode('utf-8')
		return(line)

	except Exception as e:
	    return(f"[ERREUR] > get(): {e}")


    def post(self, message: str, return: bool, delay: retry = 10) -> str:
        """
        Envoyer un message et recevoir la reponse de l'Arduino

        Args:
            - return (bool): Si l'Arduino doit retourner un message ou non.
        """

        try:
            self.serial.write(message.encode('utf-8'))

            if return:
		for i in range(retry):
                    result = self.get()
		    if result = None:
			time.sleep(0.5)
	            else:
                        return(result)
                return(f"[ERREUR] > post(): {e}")

            else:
                return(f"[ENVOYER] : {message.encode('utf-8')}

        except Exception as e:
            return(f"[ERREUR] > post(): {e}")
