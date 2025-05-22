import React, { useState } from 'react';
import Home from './Home';
import Speed from './Speed';
import Distance from './Distance';

function Navbar() {
  const [activeTab, setActiveTab] = useState("home");

  return (
    <div className="flex min-h-screen">
      <ul
        className="w-64 bg-gray-800 text-white flex flex-col py-8 px-4 space-y-2"
        role="tablist"
      >
        <li>
          <button
            onClick={() => setActiveTab("home")}
            className={`w-full text-left px-4 py-2 rounded-lg transition-all ${
              activeTab === "home" ? "bg-blue-600" : "hover:bg-gray-700"
            }`}
            role="tab"
            aria-selected={activeTab === "home"}
          >
            Home
          </button>
        </li>
        <li>
          <button
            onClick={() => setActiveTab("speed")}
            className={`w-full text-left px-4 py-2 rounded-lg transition-all ${
              activeTab === "speed" ? "bg-blue-600" : "hover:bg-gray-700"
            }`}
            role="tab"
            aria-selected={activeTab === "speed"}
          >
            Speed
          </button>
        </li>
        <li>
          <button
            onClick={() => setActiveTab("distance")}
            className={`w-full text-left px-4 py-2 rounded-lg transition-all ${
              activeTab === "distance" ? "bg-blue-600" : "hover:bg-gray-700"
            }`}
            role="tab"
            aria-selected={activeTab === "distance"}
          >
            Distance
          </button>
        </li>
      </ul>

      {/* Tab Content */}
      <div className="flex-grow bg-gray-100 p-8">
        {activeTab === "home" && (
          <div>
            <Home />
          </div>
        )}
        {activeTab === "speed" && (
          <div>
            <Speed />
          </div>
        )}
        {activeTab === "distance" && (
          <div>
            <Distance />
          </div>
        )}
      </div>
    </div>
  );
}

export default Navbar;
