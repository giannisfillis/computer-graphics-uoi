using System;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UIElements;

public class BobMove : MonoBehaviour
{
    
    public List<Vector3> map;
    public Cherry cherry;
    public bool bobCollectedCherry = false;
    public bool bobCollectedOrange = false;
    public bool bobCollectedLemon = false;
    public bool bobIsAlive = true;
    public Trap trap;
    public Logic logic;
    public Orange orange;
    public Lemon lemon;

    public Rigidbody rb;
    private float speed = 50f;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        map = logic.createMap();
        rb = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        //change speed 
        if (Input.GetKeyDown(KeyCode.Alpha1))
        {
            speed = 30f;
        }
        if (Input.GetKeyDown(KeyCode.Alpha2))
        {
            speed = 40f;
        }
        if (Input.GetKeyDown(KeyCode.Alpha3))
        {
            speed = 50f;
        }
        if (Input.GetKeyDown(KeyCode.Alpha4))
        {
            speed = 60f;
        }
        if (Input.GetKeyDown(KeyCode.Alpha5))
        {
            speed = 70f;
        }
    }

    // FixedUpdate is called based on a fixed timestep in settings
    // we use it for the physics to work propely
    void FixedUpdate()
    {
        Vector3 newPosition = transform.position;

        if (Input.GetKey(KeyCode.L)) //right
        {

            if (isAtEnd(transform.position))
            {
                transform.position = moveToStart();
            }
            else
            {
                newPosition += transform.forward * speed * Time.deltaTime;        
            }
        }

        if (Input.GetKey(KeyCode.J)) //left
        {
            if (isAtStart(transform.position))
            {
                transform.position = moveToEnd();
            }
            else
            {         
                newPosition += -transform.forward * speed * Time.deltaTime;              
            }
        }
        if (Input.GetKey(KeyCode.I)) //up
        {          
            newPosition += -transform.right * speed * Time.deltaTime;
        }
        if (Input.GetKey(KeyCode.K)) //down
        {         
            newPosition += transform.right * speed * Time.deltaTime;
        }

        if (checkBobMove(newPosition))
        {
            rb.MovePosition(newPosition);
        }
    }

 


    public bool checkBobMove(Vector3 position)
    {
        for (int i = 0; i < map.Count; i++)
        {
            if (Math.Abs(position.z - map[i].z) < 7.5f && Math.Abs(position.x - map[i].x) < 7.5f)
            {
                Debug.Log("wall hit - colliding with "+ map[i].x+","+ map[i].z);
                return false;
            }
        }
        return true;
    }

    private bool isAtStart(Vector3 position)
    {
        if (position.x < -20 && position.z < -35)
        {
            return true;
        }
        return false;
    }

    private bool isAtEnd(Vector3 position)
    {
        if (position.x > 20 && position.z > 45)
        {
            return true;
        }
        return false;
    }

    private Vector3 moveToStart()
    {
        return new Vector3(-25, 5, -40);
    }

    private Vector3 moveToEnd()
    {
        return new Vector3(25, 5, 50);
    }

    public int checkIfBobCollectedTreasure(Vector3 bobPosition)
    {
        //collected cherry
        if (Math.Abs(bobPosition.z - cherry.transform.position.z) < 5f && Math.Abs(bobPosition.x - cherry.transform.position.x) < 5f)
        {
            return 1;
        }
        //collected orange
        if (Math.Abs(bobPosition.z - orange.transform.position.z) < 5f && Math.Abs(bobPosition.x - orange.transform.position.x) < 5f)
        {
            return 2;
        }
        //collected lemon
        if (Math.Abs(bobPosition.z - lemon.transform.position.z) < 5f && Math.Abs(bobPosition.x - lemon.transform.position.x) < 5f)
        {
            return 3;
        }
        return 0;
    }

    public bool checkIfBobFellIntoTrap(Vector3 bobPosition)
    {
        if (Math.Abs(bobPosition.z - trap.transform.position.z) < 5f && Math.Abs(bobPosition.x - trap.transform.position.x) < 5f)
        {
            return true;
        }
        return false;
    }

    /*
        Vector3 newPosition = transform.position;
        

        if (Input.GetKeyDown(KeyCode.L)) //right
        { 

            if (isAtEnd(transform.position))
            {
                newPosition = moveToStart();
            }
            else
            {
                newPosition += new Vector3(0, 0, moveStep);
            }
        }
  
        if (Input.GetKeyDown(KeyCode.J)) //left
        {
            if (isAtStart(transform.position))
            {
                newPosition = moveToEnd();
            }
            else
            {
                newPosition += new Vector3(0, 0, -moveStep);
            }
        }
        if (Input.GetKeyDown(KeyCode.I)) //up
        {
            newPosition += new Vector3(-moveStep, 0, 0);
        }
        if (Input.GetKeyDown(KeyCode.K)) //down
        {
            newPosition += new Vector3(moveStep, 0, 0);
        }


        if (checkBobMove(newPosition))
        {
            transform.position = newPosition;
        }
        */


    //newPosition += transform.forward * speed * Time.deltaTime;
    //movement += transform.forward;
    //movement += transform.forward * speed * Time.deltaTime;
    //transform.Translate(Vector3.forward*speed*Time.deltaTime);
  
    //rb.MovePosition(newPosition + transform.forward * speed *


}
