// contains functions responsible for making HTTP requests to backend server
import axios from 'axios';

const BASE_URL = 'http://localhost:8000'; // Replace with your actual backend API URL

export const sendDataToBackend = async (data) => {
  try {
    const response = await axios.post(`${BASE_URL}/api/validate`, data);
    return response.data;
  } catch (error) {
    throw error;
  }
};
