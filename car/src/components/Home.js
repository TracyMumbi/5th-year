import React from 'react';

function Home() {
  return (
    <div
      className="min-h-screen bg-cover bg-center relative"
      style={{
        backgroundImage: `url('https://images.unsplash.com/photo-1705771802714-a3735fc6fc96?w=500&auto=format&fit=crop&q=60&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxzZWFyY2h8M3x8Y3liZXJ0cnVja3xlbnwwfHwwfHx8MA%3D%3D')`,
        backgroundSize: 'cover',
        backgroundPosition: 'center',
        height: '100vh', 
      }}
    >
      <div className="absolute inset-0 bg-black bg-opacity-50"></div>

      <div
        className="relative z-10 flex items-center justify-center min-h-screen px-6"
        style={{
          textAlign: 'center', 
        }}
      >
        <div
          className="text-white max-w-3xl text-center"
          style={{
            padding: '20px', 
            backgroundColor: 'rgba(0, 0, 0, 0.5)', 
            borderRadius: '10px', 
          }}
        >
          <h1
            className="text-4xl md:text-5xl font-bold mb-6"
            style={{
              fontWeight: 'bold',
              marginBottom: '20px',
              textShadow: '2px 2px 4px rgba(0, 0, 0, 0.7)', 
            }}
          >
            Cruise Control in a Model Car
          </h1>
          <p
            className="text-lg md:text-xl leading-relaxed"
            style={{
              fontSize: '1.2rem',
              lineHeight: '1.8',
              fontFamily: "'Arial', sans-serif", 
            }}
          >
            Our project focuses on the design and fabrication of a cruise control system
            integrated into a model car. The system utilizes line following for guided
            navigation, obstacle detection for enhanced safety, and speed regulation for
            consistent motion—demonstrating smart vehicle automation on a small scale.
          </p>
        </div>
      </div>
    </div>
  );
}

export default Home;
