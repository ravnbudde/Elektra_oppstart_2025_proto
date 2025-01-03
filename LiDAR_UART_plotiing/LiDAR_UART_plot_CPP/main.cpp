//
// This is example code from Chapter 2.2 "The classic first program" of
// "Programming -- Principles and Practice Using C++" by Bjarne Stroustrup
//
// This program outputs the message "Hello, World!" to the monitor

#include "std_lib_facilities.h"

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cmath>
#include <map>
#include <deque>
#include <boost/asio.cpp>
#include <SFML/Graphics.cpp> // For visualisering
#include <SFML/System.cpp>
#include <SFML/Window.cpp>
#include <SFML/Network.cpp> // For UART

// Kalman-filter
class KalmanFilter {
private:
    double process_variance;
    double measurement_variance;
    double estimate;
    double uncertainty;

public:
    KalmanFilter(double process_var, double measurement_var)
        : process_variance(process_var), measurement_variance(measurement_var), estimate(0), uncertainty(1) {}

    double update(double measurement) {
        double kalman_gain = uncertainty / (uncertainty + measurement_variance);
        estimate = estimate + kalman_gain * (measurement - estimate);
        uncertainty = (1 - kalman_gain) * uncertainty;
        return estimate;
    }

    void predict(double control_input = 0) {
        estimate += control_input;
        uncertainty += process_variance;
    }
};

// LiDAR-filtrering
class LiDARFiltering {
private:
    const double distance_limit;
    const double distance_offset;
    std::map<int, KalmanFilter> kalman_filters;
    std::map<int, sf::Vector2f> filtered_points;
    std::mutex lock;

public:
    LiDARFiltering(double distance_lim, double distance_off, double process_var, double measurement_var)
        : distance_limit(distance_lim), distance_offset(distance_off) {
        for (int i = 0; i < 360; ++i) {
            kalman_filters[i] = KalmanFilter(process_var, measurement_var);
        }
    }

    void processData(int angle, double distance) {
        if (distance <= distance_limit) {
            double rad_angle = (angle + 90) * M_PI / 180.0;
            double corrected_distance = distance + distance_offset;

            double x = 400 + corrected_distance * cos(rad_angle) * 5;
            double y = 400 - corrected_distance * sin(rad_angle) * 5;

            lock.lock();
            double filtered_distance = kalman_filters[angle].update(distance);

            double filtered_x = 400 + (filtered_distance + distance_offset) * cos(rad_angle) * 5;
            double filtered_y = 400 - (filtered_distance + distance_offset) * sin(rad_angle) * 5;

            filtered_points[angle] = sf::Vector2f(filtered_x, filtered_y);
            lock.unlock();
        }
    }

    std::map<int, sf::Vector2f> getFilteredPoints() {
        std::lock_guard<std::mutex> guard(lock);
        return filtered_points;
    }
};

// Visualisering
class LiDARVisualizer {
private:
    sf::RenderWindow window;
    LiDARFiltering& filtering;

public:
    LiDARVisualizer(LiDARFiltering& filter)
        : filtering(filter), window(sf::VideoMode(800, 800), "LiDAR Visualisering") {}

    void drawPoints(const std::map<int, sf::Vector2f>& points) {
        for (const auto& point : points) {
            sf::CircleShape circle(2);
            circle.setPosition(point.second);
            circle.setFillColor(sf::Color::Green);
            window.draw(circle);
        }
    }

    void run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear();
            auto points = filtering.getFilteredPoints();
            drawPoints(points);
            window.display();
        }
    }
};

// UART-lesing
void uartReader(LiDARFiltering& filtering) {
    using namespace boost::asio;
    io_service io;
    serial_port serial(io, "/dev/ttyUSB0"); // Bytt ut med korrekt port for ditt system (f.eks. COM3 på Windows)

    serial.set_option(serial_port_base::baud_rate(115200));
    serial.set_option(serial_port_base::character_size(8));
    serial.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
    serial.set_option(serial_port_base::parity(serial_port_base::parity::none));
    serial.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));

    char buffer[256];
    while (true) {
        try {
            std::size_t bytes_read = read(serial, buffer(buffer), boost::asio::transfer_at_least(1));
            std::string data(buffer, bytes_read);

            // Ekstrakt vinkel og avstand frå meldinga
            std::stringstream ss(data);
            std::string angle_str, distance_str;
            if (std::getline(ss, angle_str, ',') && std::getline(ss, distance_str)) {
                int angle = std::stoi(angle_str);
                double distance = std::stod(distance_str);

                // Send data til filtrering
                filtering.processData(angle, distance);
            }
        } catch (std::exception& e) {
            std::cerr << "UART-lesing feilet: " << e.what() << std::endl;
        }

        // Kontroller oppdateringsfrekvens
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    LiDARFiltering lidar_filtering(60, 2, 0.1, 1.0);
    std::thread uart_thread(uartReader, std::ref(lidar_filtering));

    LiDARVisualizer visualizer(lidar_filtering);
    visualizer.run();

    uart_thread.join();
    return 0;
}
