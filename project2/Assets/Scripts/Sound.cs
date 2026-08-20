using UnityEngine;

public class Sound : MonoBehaviour
{
    public AudioSource asrc;
    public AudioClip coin;
    public AudioClip death;

    public void PlayCoinSound()
    {
        asrc.clip = coin;
        asrc.Play();
    }

    public void PlayDeathSound()
    {
        asrc.clip = death;
        asrc.Play();
    }
}
