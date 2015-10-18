using UnityEngine;
using System.Collections;

public class motorScript : MonoBehaviour {

	public double power = 0.0f;

	void FixedUpdate () {
	
		GetComponent<Rigidbody> ().AddRelativeForce (0, (float)power, 0);

	}
}
