using UnityEngine;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class Logic : MonoBehaviour
{
    public BobMove bob;
    public Cherry cherry;
    public Trap trap;
    public Orange orange;
    public Lemon lemon;
    public Sound audio;
    public ParticleSystem collectionEffect;
    public ParticleSystem deathEffect;
    public Text scoreText;
    public int playerScore;
    public GameObject gameOverScreen;
    

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        collectionEffect = bob.GetComponent<ParticleSystem>();
        deathEffect = trap.GetComponent<ParticleSystem>();
    }

    // Update is called once per frame
    void Update()
    {
        //bob collected cherry
        if (bob.checkIfBobCollectedTreasure(bob.transform.position) == 1 && bob.bobCollectedCherry == false)
        {
            audio.PlayCoinSound();
            collectionEffect.Play();
            Debug.Log("cherry collected");
            addScore(cherry.cherryScore);
            bob.bobCollectedCherry = true;
            cherry.shrinkTreasure();
            cherry.cherryShrinked = false;
        }
        //bob collected orange
        else if (bob.checkIfBobCollectedTreasure(bob.transform.position) == 2 && bob.bobCollectedOrange == false)
        {
            audio.PlayCoinSound();
            collectionEffect.Play();
            Debug.Log("orange collected");
            addScore(orange.orangeScore);
            bob.bobCollectedOrange = true;
            orange.shrinkTreasure();
            orange.orangeShrinked = false;
        }
        //bob collected lemon
        else if (bob.checkIfBobCollectedTreasure(bob.transform.position) == 3 && bob.bobCollectedLemon == false)
        {
            audio.PlayCoinSound();
            collectionEffect.Play();
            Debug.Log("lemon collected");
            addScore(lemon.lemonScore);
            bob.bobCollectedLemon = true;
            lemon.shrinkTreasure();
            lemon.lemonShrinked = false;
        }
        //bob fell into trap
        if (bob.checkIfBobFellIntoTrap(bob.transform.position) && bob.bobIsAlive == true)
        {
            audio.PlayDeathSound();
            deathEffect.Play();
            bob.bobIsAlive = false;
            gameOver();
        }
    }

    //create a list with vectors of each wall block position
    public List<Vector3> createMap()
    {
        List<Vector3> map = new List<Vector3>();
        int k = -40;
        for (int i = 0; i < 10; i++)
        {
            map.Add(new Vector3(-45, 5, k));
            k += 10;
        }

        map.Add(new Vector3(-35, 5, -40));

        int l = -15;
        for (int i = 0; i < 7; i++)
        {
            map.Add(new Vector3(l, 5, -40));
            l += 10;
        }

        int j = -30;
        for (int i = 0; i < 9; i++)
        {
            map.Add(new Vector3(45, 5, j));
            j += 10;
        }

        int h = -35;
        for (int i = 0; i < 6; i++)
        {
            map.Add(new Vector3(h, 5, 50));
            h += 10;
        }

        map.Add(new Vector3(35, 5, -20));
        map.Add(new Vector3(35, 5, 0));
        map.Add(new Vector3(35, 5, 10));
        map.Add(new Vector3(35, 5, 50));
        map.Add(new Vector3(15, 5, -20));
        map.Add(new Vector3(25, 5, 30));
        map.Add(new Vector3(15, 5, -10));
        map.Add(new Vector3(-5, 5, 0));
        map.Add(new Vector3(-5, 5, 10));
        map.Add(new Vector3(5, 5, 10));
        map.Add(new Vector3(5, 5, 10));
        map.Add(new Vector3(15, 5, 10));
        map.Add(new Vector3(15, 5, 20));
        map.Add(new Vector3(15, 5, 30));

        int f = -25;
        for (int i = 0; i < 3; i++)
        {
            map.Add(new Vector3(f, 5, 30));
            f += 10;
        }

        int g = -25;
        for (int i = 0; i < 3; i++)
        {
            map.Add(new Vector3(g, 5, -20));
            g += 10;
        }

        int p = -10;
        for (int i = 0; i < 3; i++)
        {
            map.Add(new Vector3(-25, 5, p));
            p += 10;
        }


        return map;
    }

    public void gameOver()
    {
        GameObject.FindGameObjectWithTag("Bob").SetActive(false);
        gameOverScreen.SetActive(true);
    }

    public void addScore(int score)
    {
        playerScore += score;
        scoreText.text = playerScore.ToString();
    }

    public void restartGame()
    {
        SceneManager.LoadScene(SceneManager.GetActiveScene().name);
    }

    public void quitGame()
    {
        Debug.Log("Game exited");
        Application.Quit();
    }

   
}
