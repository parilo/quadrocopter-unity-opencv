using UnityEngine;
using System.Collections;

public class motorScript : MonoBehaviour {

	public double power = 0.0f;
	public double powerLimit = 40.0d;

	void FixedUpdate () {
	
		if (power > powerLimit) {
			power = powerLimit;
		} else if (power < -powerLimit) {
			power = -powerLimit;
		}

		GetComponent<Rigidbody> ().AddRelativeForce (0, (float)power, 0);

	}
}
