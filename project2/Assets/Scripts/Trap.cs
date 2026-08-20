using System;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UIElements;
using Random = UnityEngine.Random;

public class Trap : MonoBehaviour
{

    public BobMove bob;
    public float spawnRate = 1.5f;
    private float timer = 0f;
    public List<Vector3> map;
    public Cherry cherry;
    public Logic logic;
    public Orange orange;
    public Lemon lemon;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        map = logic.createMap();
        spawnTrap();
    }

    // Update is called once per frame
    void Update()
    {
        if (timer < spawnRate)
        {
            timer += Time.deltaTime;
        }
        else
        {
            if (bob.bobIsAlive)
            {
                spawnTrap();
                Debug.Log("trap spawned");
                timer = 0f;
            }
        }
    }

    public void spawnTrap()
    {
        bool valid = false;
        Vector3 newPosition = transform.position;
        spawnRate = Random.Range(0.5f, 4f);

        while (valid == false)
        {
            int rndx = Random.Range(-4, 4);
            if (rndx < 0)
            {
                //to match map coordinates
                rndx = rndx * 10 - 5;
            }
            else if (rndx > 0)
            {
                //to match map coordinates
                rndx = rndx * 10 + 5;
            }
            else if (rndx == 0)
            {
                //to match map coordinates
                rndx = 5;
            }

            int rndz = Random.Range(-4, 4);
            rndz = rndz * 10;


            newPosition = new Vector3(rndx, 5, rndz);

            if (checkTrapMove(newPosition))
            {
                transform.position = newPosition;
                valid = true;
            }
        }
    }

    private bool checkTrapMove(Vector3 position)
    {
        //if new position is the previous position
        if (transform.position == position)
        {
            return false;
        }
        //if new position is in a wall
        if (map.Contains(position))
        {
            return false;
        }
        //if new position is bob's position
        if (Math.Abs(bob.transform.position.z - position.z) < 5f && Math.Abs(bob.transform.position.x - position.x) < 5f)
        {
            return false;
        }
        // if new position is a treasure's position
        if (cherry.transform.position == position || orange.transform.position == position || lemon.transform.position == position)
        {
            return false;
        }

        return true;
    }
}
