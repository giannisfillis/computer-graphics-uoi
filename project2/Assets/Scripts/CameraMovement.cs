using System;
using System.Collections.Generic;
using UnityEngine;

public class CameraMovement : MonoBehaviour
{

    private Vector3 cameraPosition;
    private float moveStep;
    
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        cameraPosition = this.transform.position;
        moveStep = 25f * Time.deltaTime;
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKey(KeyCode.RightArrow)) // right move, +z
        {
            cameraPosition += new Vector3(0, 0, moveStep);
        }
        else if (Input.GetKey(KeyCode.LeftArrow)) // left move, -z
        {
            cameraPosition += new Vector3(0, 0, -moveStep);
        }
        else if (Input.GetKey(KeyCode.UpArrow)) // up move, +x
        {
            cameraPosition += new Vector3(-moveStep, 0,0);
        }
        else if (Input.GetKey(KeyCode.DownArrow)) // down move, -x
        {
            cameraPosition += new Vector3(moveStep, 0, 0);
        }
        else if (Input.GetKey(KeyCode.KeypadPlus)) // zoom in, -y
        {
            cameraPosition += new Vector3(0, -moveStep, 0);
        }
        else if (Input.GetKey(KeyCode.KeypadMinus)) // zoom in, +y
        {
            cameraPosition += new Vector3(0, moveStep, 0);
        }
        transform.position = cameraPosition;
        if (Input.GetKey(KeyCode.R))
        {
            transform.Rotate((new Vector3(0, moveStep, 0)), Space.Self);
        }
    }
    
}
