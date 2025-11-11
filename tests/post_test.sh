#!/bin/bash

curl -X POST "http://localhost:8080/echo/post/json?First+name=Hello&amp;Last+name=World&amp;Age=10" -H "Content-Type: application/json" --data-binary @- <<DATA
{
  "Id": 12345,
  "Customer": "John Smith",
  "Quantity": 1,
  "Price": 10.00
}
DATA

