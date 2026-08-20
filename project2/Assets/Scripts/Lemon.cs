using System;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using Random = UnityEngine.Random;

public class Lemon : MonoBehaviour
{
    public BobMove bob;
    public Trap trap;
    public float spawnRate = 4f;
    private float timer = 0f;
    public List<Vector3> map;
    public bool lemonShrinked = false;
    public Logic logic;
    public Orange orange;
    public Cherry cherry;
    public int lemonScore = 1;
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
                Debug.Log("lemon spawned");
                timer = 0f;
                bob.bobCollectedLemon = false;
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
        //trap or treasure
        if (trap.transform.position == position || orange.transform.position == position || cherry.transform.position == position)
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
        if (lemonShrinked == false)
        {
            transform.localScale = new Vector3(transform.localScale.x / 2, transform.localScale.y / 2, transform.localScale.z / 2);
        }

        lemonShrinked = true;
    }
}