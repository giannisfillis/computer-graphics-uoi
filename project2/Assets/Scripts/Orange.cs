using System;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using Random = UnityEngine.Random;

public class Orange : MonoBehaviour
{
    public BobMove bob;
    public Trap trap;
    public float spawnRate = 3f;
    private float timer = 0f;
    public List<Vector3> map;
    public bool orangeShrinked = false;
    public Logic logic;
    public Cherry cherry;
    public Lemon lemon;
    public int orangeScore = 2;
    //public bool bobCollectedTreasure = false;


    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        map = logic.createMap();
        spawnTreasure();
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
                spawnTreasure();
                Debug.Log("orange spawned");
                timer = 0f;
                bob.bobCollectedOrange = false;
            }
        }

    }

    public void spawnTreasure()
    {
        bool valid = false;
        Vector3 newPosition = transform.position;
        transform.localScale = new Vector3(5, 5, 5);

        while (valid == false)
        {
            int rndx = Random.Range(-4, 4);
            if (rndx < 0)
            {
                rndx = rndx * 10 - 5;
            }
            else if (rndx > 0)
            {
                rndx = rndx * 10 + 5;
            }
            else if (rndx == 0)
            {
                rndx = 5;
            }

            int rndz = Random.Range(-4, 4);
            rndz = rndz * 10;


            newPosition = new Vector3(rndx, 5, rndz);

            if (checkTreasureMove(newPosition))
            {
                transform.position = newPosition;
                valid = true;
                //bob.bobCollectedTreasure = false;
            }
        }

    }

    private bool checkTreasureMove(Vector3 position)
    {
        //previous position
        if (transform.position == position)
        {
            return false;
        }
        //wall
        if (map.Contains(position))
        {
            return false;
        }
        //trap or other treasure
        if (trap.transform.position == position || cherry.transform.position == position || lemon.transform.position == position)
        {
            return false;
        }
        //bob
        if (Math.Abs(bob.transform.position.z - position.z) < 5f && Math.Abs(bob.transform.position.x - position.x) < 5f)
        {
            return false;
        }
        return true;
    }

    public void shrinkTreasure()
    {
        if (orangeShrinked == false)
        {
            transform.localScale = new Vector3(transform.localScale.x / 2, transform.localScale.y / 2, transform.localScale.z / 2);
        }

        orangeShrinked = true;

    }
}
