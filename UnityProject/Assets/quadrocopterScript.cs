using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System;

public class quadrocopterScript : MonoBehaviour {

	//фактические параметры
	private double pitch; //Тангаж
	private double roll; //Крен
	private double yaw; //Рыскание
	public double throttle; //Газ, газ мы задаем извне, поэтому он public

	//требуемые параметры
	public double targetPitch;
	public double targetRoll;
	public double targetYaw;

	//PID регуляторы, которые будут стабилизировать углы
	//каждому углу свой регулятор, класс PID определен ниже
	//константы подобраны на глаз :) пробуйте свои значения
	private PID pitchPID = new PID (100, 0, 20);
	private PID rollPID = new PID (100, 0, 20);
	private PID yawPID = new PID (50, 0, 50);

	private Quaternion prevRotation = new Quaternion (0, 1, 0, 0);

//	private double motor1power = 0;
//	private double motor2power = 0;
//	private double motor3power = 0;
//	private double motor4power = 0;
	private motorScript[] motors;

	private Transform frameTransform;

	void Awake () {
		motors = new motorScript[4];
		motors [0] = GameObject.Find ("Motor1").GetComponent<motorScript> ();
		motors [1] = GameObject.Find ("Motor2").GetComponent<motorScript> ();
		motors [2] = GameObject.Find ("Motor3").GetComponent<motorScript> ();
		motors [3] = GameObject.Find ("Motor4").GetComponent<motorScript> ();
		incMotorPower (0, 22);
		incMotorPower (1, 22);
		incMotorPower (2, 22);
		incMotorPower (3, 22);

		frameTransform = GameObject.Find ("Frame").GetComponent<Transform> ();
	}

	void readRotation () {
		
		//фактическая ориентация нашего квадрокоптера,
		//в реальном квадрокоптере эти данные необходимо получать
		//из акселерометра-гироскопа-магнетометра, так же как делает это ваш
		//смартфон
		Vector3 rot = GameObject.Find ("Frame").GetComponent<Transform> ().rotation.eulerAngles;
		pitch = rot.x;
		yaw = rot.y;
		roll = rot.z;

	}

	public Quaternion getRotation () {
		return frameTransform.rotation;
	}

	public Vector3 getPosition () {
		return frameTransform.position;
	}

	public Vector4 getMotorsPower () {
		return new Vector4 (
			(float) motors [0].power,
			(float) motors [1].power,
			(float) motors [2].power,
			(float) motors [3].power
		);
	}

	public void incMotorPower (int motorIndex, double delta) {
		motors [motorIndex].power += delta;
	}

	public void setMotorsPower (double power) {
		motors [0].power = power;
		motors [1].power = power;
		motors [2].power = power;
		motors [3].power = power;
	}

	//функция стабилизации квадрокоптера
	//с помощью PID регуляторов мы настраиваем
	//мощность наших моторов так, чтобы углы приняли нужные нам значения
	void stabilize () {

		//нам необходимо посчитать разность между требуемым углом и текущим
		//эта разность должна лежать в промежутке [-180, 180] чтобы обеспечить
		//правильную работу PID регуляторов, так как нет смысла поворачивать на 350
		//градусов, когда можно повернуть на -10

		double dPitch = targetPitch - pitch;
		double dRoll = targetRoll - roll;
		double dYaw = targetYaw - yaw;

		dPitch -= Math.Ceiling (Math.Floor (dPitch / 180.0) / 2.0) * 360.0;
		dRoll -= Math.Ceiling (Math.Floor (dRoll / 180.0) / 2.0) * 360.0;
		dYaw -= Math.Ceiling (Math.Floor (dYaw / 180.0) / 2.0) * 360.0;

		//1 и 2 мотор впереди
		//3 и 4 моторы сзади
		double motor1power = throttle;
		double motor2power = throttle;
		double motor3power = throttle;
		double motor4power = throttle;

		//ограничитель на мощность подаваемую на моторы,
		//чтобы в сумме мощность всех моторов оставалась
		//одинаковой при регулировке
		double powerLimit = throttle > 20 ? 20 : throttle;

		//управление тангажем:
		//на передние двигатели подаем возмущение от регулятора
		//на задние противоположное возмущение
		double pitchForce = - pitchPID.calc (0, dPitch / 180.0);
		pitchForce = pitchForce > powerLimit ? powerLimit : pitchForce;
		pitchForce = pitchForce < -powerLimit ? -powerLimit : pitchForce;
		motor1power +=   pitchForce;
		motor2power +=   pitchForce;
		motor3power += - pitchForce;
		motor4power += - pitchForce;

		//управление креном:
		//действуем по аналогии с тангажем, только регулируем боковые двигатели
		double rollForce = - rollPID.calc (0, dRoll / 180.0);
		rollForce = rollForce > powerLimit ? powerLimit : rollForce;
		rollForce = rollForce < -powerLimit ? -powerLimit : rollForce;
		motor1power +=   rollForce;
		motor2power += - rollForce;
		motor3power += - rollForce;
		motor4power +=   rollForce;

		//управление рысканием:
		double yawForce = yawPID.calc (0, dYaw / 180.0);
		yawForce = yawForce > powerLimit ? powerLimit : yawForce;
		yawForce = yawForce < -powerLimit ? -powerLimit : yawForce;
		motor1power +=   yawForce;
		motor2power += - yawForce;
		motor3power +=   yawForce;
		motor4power += - yawForce;

		GameObject.Find ("Motor1").GetComponent<motorScript>().power = motor1power;
		GameObject.Find ("Motor2").GetComponent<motorScript>().power = motor2power;
		GameObject.Find ("Motor3").GetComponent<motorScript>().power = motor3power;
		GameObject.Find ("Motor4").GetComponent<motorScript>().power = motor4power;
	}

	//как советуют в доке по Unity вычисления проводим в FixedUpdate, а не в Update
//	void FixedUpdate () {
//		readRotation ();
//		stabilize ();
//	}
	
}

public class PID {
	
	private double P;
	private double I;
	private double D;
	
	private double prevErr;
	private double sumErr;
	
	public PID (double P, double I, double D) {
		this.P = P;
		this.I = I;
		this.D = D;
	}
	
	public double calc (double current, double target) {
		
		double dt = Time.fixedDeltaTime;
		
		double err = target - current;
		this.sumErr += err;
		
		double force = this.P * err + this.I * this.sumErr * dt + this.D * (err - this.prevErr) / dt;
		
		this.prevErr = err;
		return force;
	}
	
};
