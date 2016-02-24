using UnityEngine;
using System.Collections;

public class motorScript : MonoBehaviour {

	public double power = 0.0f;
	public double powerLimit = 24.0d;

	void FixedUpdate () {
	
		if (power > powerLimit) {
			power = powerLimit;
		} else if (power < 0) {
			power = 0;
		}
//		} else if (power < -powerLimit) {
//			power = -powerLimit;
//		}

		GetComponent<Rigidbody> ().AddRelativeForce (0, (float)power, 0);

	}
}
