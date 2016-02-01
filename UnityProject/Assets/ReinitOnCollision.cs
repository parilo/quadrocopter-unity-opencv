using UnityEngine;
using System.Collections;

public class ReinitOnCollision : MonoBehaviour {

	private Rigidbody frame;
	private Rigidbody motor1;
	private Rigidbody motor2;
	private Rigidbody motor3;
	private Rigidbody motor4;

	private quadrocopterScript quadrocopter;

	private GameManager gameManager;

	void Start () {

		quadrocopter = GameObject.Find ("/Quadrocopter").GetComponent <quadrocopterScript> ();
		gameManager = GameObject.Find ("/GameManager").GetComponent <GameManager> ();

		frame = GameObject.Find ("/Quadrocopter/Frame").GetComponent <Rigidbody> ();
		motor1 = GameObject.Find ("/Quadrocopter/Motor1").GetComponent <Rigidbody> ();
		motor2 = GameObject.Find ("/Quadrocopter/Motor2").GetComponent <Rigidbody> ();
		motor3 = GameObject.Find ("/Quadrocopter/Motor3").GetComponent <Rigidbody> ();
		motor4 = GameObject.Find ("/Quadrocopter/Motor4").GetComponent <Rigidbody> ();
	}

	void resetQuadrocopter () {

		frame.MovePosition (new Vector3 (0, 4, 0));
		frame.MoveRotation (Quaternion.Euler (0, 0, 0));
		frame.velocity = new Vector3(0, 0, 0);

		motor1.MovePosition (new Vector3 (-0.22f, 4, 0.22f));
		motor1.MoveRotation (Quaternion.Euler (10, 45, 0));
		motor1.velocity = new Vector3(0, 0, 0);

		motor2.MovePosition (new Vector3 (0.22f, 4, 0.22f));
		motor2.MoveRotation (Quaternion.Euler (0, 45, 10));
		motor2.velocity = new Vector3(0, 0, 0);

		motor3.MovePosition (new Vector3 (0.22f, 4, -0.22f));
		motor3.MoveRotation (Quaternion.Euler (350, 45, 0));
		motor3.velocity = new Vector3(0, 0, 0);

		motor4.MovePosition (new Vector3 (-0.22f, 4, -0.22f));
		motor4.MoveRotation (Quaternion.Euler (0, 45, 350));
		motor4.velocity = new Vector3(0, 0, 0);

		quadrocopter.setMotorsPower (22);
		gameManager.resetPrevAction ();
	}
	
	void OnCollisionEnter (Collision col) {
		resetQuadrocopter ();
	}

	void Update () {
		Vector3 p = quadrocopter.getPosition ();
		if (
			Mathf.Abs (p.x) > 10 ||
			Mathf.Abs (p.y) > 10 ||
			Mathf.Abs (p.z) > 10)
		{
			resetQuadrocopter ();
		}
	}

}
