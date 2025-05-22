import React, { useState, useEffect } from 'react';
import {
  AreaChart,
  Area,
  ResponsiveContainer,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  Legend,
} from 'recharts';

function Speed() {
  const [speed, setSpeed] = useState('');
  const [submittedSpeed, setSubmittedSpeed] = useState(null);
  const [speedData, setSpeedData] = useState([]);
  const [loading, setLoading] = useState(true);

  const fetchspeedData = async () => {
    try {
      const response = await fetch('http://172.20.10.3:3000/parameters');
      if (!response.ok) {
        throw new Error('Failed to fetch speed data');
      }
      const data = await response.json();
      console.log('Fetched data:', data);

      // Create a fake "time" value based on index or if your backend has timestamps, use those
      const formattedData = data.map((item, index) => ({
        ...item,
        time: `${index * 5}s`, // Every 5 seconds as an example
      }));

      setSpeedData(formattedData);
      setLoading(false);
    } catch (error) {
      console.error('Error fetching speed data:', error);
      setLoading(false);
    }
  };

  useEffect(() => {
    const interval = setInterval(() => {
      fetchspeedData();
    }, 5000);

    fetchspeedData();

    return () => clearInterval(interval);
  }, []);

  const handleSubmit = async (e) => {
    e.preventDefault();
    if (speed.trim() !== '') {
      try {
        const response = await fetch('http://172.20.10.3:3000/parameters', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({ speed }),
        });
        if (!response.ok) {
          throw new Error('Failed to submit speed');
        }
        setSubmittedSpeed(speed);
        setSpeed('');
        fetchspeedData();
      } catch (error) {
        console.error('Error submitting speed:', error);
      }
    }
  };

  return (
    <div className="p-8 max-w-4xl mx-auto bg-white shadow-md rounded-lg mt-8">
      <h2 className="text-2xl font-bold mb-6 text-blue-600 text-center">Car speed Over Time Tracker</h2>

      <form onSubmit={handleSubmit} className="space-y-4 mb-8">
        <textarea
          className="w-full h-24 p-3 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-400 resize-none"
          placeholder="Enter current car speed (e.g., 120 cm)"
          value={speed}
          onChange={(e) => setSpeed(e.target.value)}
        />

        <button
          type="submit"
          className="px-6 py-2 bg-blue-600 text-white rounded-md hover:bg-blue-700 transition-all"
        >
          Send
        </button>
      </form>

      {submittedSpeed && (
        <div className="mb-10 bg-blue-50 border-l-4 border-blue-500 text-blue-700 p-4 rounded">
          <p className="font-semibold">Submitted speed:</p>
          <p>{submittedSpeed}</p>
        </div>
      )}

      <h3 className="text-xl font-semibold mb-4 text-gray-700">speed Over Time</h3>

      <div className="h-80 w-full">
        {loading ? (
          <p>Loading...</p>
        ) : (
          <ResponsiveContainer width="100%" height="100%">
            <AreaChart data={speedData} margin={{ top: 10, right: 30, left: 0, bottom: 0 }}>
              <defs>
                <linearGradient id="speedColor" x1="0" y1="0" x2="0" y2="1">
                  <stop offset="5%" stopColor="#3b82f6" stopOpacity={0.8} />
                  <stop offset="95%" stopColor="#3b82f6" stopOpacity={0} />
                </linearGradient>
              </defs>
              <XAxis dataKey="time" label={{ value: 'Time', position: 'insideBottom', dy: 10 }} />
              <YAxis
                label={{ value: 'speed (cm)', angle: -90, position: 'insideLeft', dx: -10 }}
              />
              <CartesianGrid strokeDasharray="3 3" />
              <Tooltip />
              <Legend />
              <Area
                type="monotone"
                dataKey="speed"
                stroke="#2563eb"
                fillOpacity={1}
                fill="url(#speedColor)"
              />
            </AreaChart>
          </ResponsiveContainer>
        )}
      </div>
    </div>
  );
}

export default Speed;
