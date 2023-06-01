import React, { useState, useEffect } from "react";
// @ts-ignore
import Trend from 'react-trend';

const Data = () => {
    const [x, setX] = useState([0]); // Initialize x as an array
    const [y, setY] = useState([0]); // Initialize x as an array
    const [z, setZ] = useState([0]); // Initialize x as an array


    const [count, setCount] = useState(0);


    useEffect(() => {
        setCount(count + 1);
        fetch("http://100.27.22.95:3000/gyr")
            .then((res) => res.json())
            .then((data) => {
                console.log(data);
                const d = JSON.parse(data);
                setX((prevX) => [...prevX, Number(d["gyroX"])]);
                setY((prevY) => [...prevY, Number(d["gyroY"])]);
                setZ((prevZ) => [...prevZ, Number(d["gyroZ"])]);

            })

            .catch((error) => {
                console.log("Error:", error);
            });
    }); // Add an empty dependency array to run the effect only once

    return (
        <div>
            <p>X:</p>
            <Trend smooth radius={20} strokeWidth={4}  padding={18}  width={1500} height={300} gradient={['#0FF', '#F0F', '#FF0']} data={x} />
            <p>Y:</p>
            <Trend smooth radius={20} strokeWidth={4}  padding={18}  width={1500} height={300} gradient={['#0FF', '#F0F', '#FF0']} data={y} />
            <p>Z:</p>
            <Trend smooth radius={20} strokeWidth={4}  padding={18}  width={1500} height={300} gradient={['#0FF', '#F0F', '#FF0']} data={z} />



            <h6>{count}</h6>
        </div>
    );
};

export default Data;
