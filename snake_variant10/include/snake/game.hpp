#pragma once

#include <cstdint>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace snake {

struct Point {
    int x{};
    int y{};
    friend bool operator==(const Point&, const Point&) = default;
};

enum class Direction { Up, Down, Left, Right };
enum class GameStatus { Menu, Playing, Paused, GameOver, Exit };
enum class EventType { StateChanged, ScoreChanged, FoodEaten, Collision };

struct GameEvent {
    EventType type{};
    GameStatus status{};
    int score{};
    std::string message;
};

class Observer {
public:
    virtual ~Observer() = default;
    virtual void on_event(const GameEvent& event) = 0;
};

class Subject {
public:
    void subscribe(Observer& observer);
    void unsubscribe(Observer& observer);
protected:
    void notify(const GameEvent& event);
private:
    std::vector<Observer*> observers_;
};

class Game : public Subject {
public:
    explicit Game(int width = 24, int height = 14, std::uint32_t seed = 0);
    void start();
    void reset();
    bool turn(Direction direction);
    bool advance();
    void set_status(GameStatus status, std::string message = {});

    [[nodiscard]] const std::vector<Point>& snake() const noexcept { return snake_; }
    [[nodiscard]] Point food() const noexcept { return food_; }
    [[nodiscard]] int score() const noexcept { return score_; }
    [[nodiscard]] int width() const noexcept { return width_; }
    [[nodiscard]] int height() const noexcept { return height_; }
    [[nodiscard]] GameStatus status() const noexcept { return status_; }
    [[nodiscard]] Direction direction() const noexcept { return direction_; }
    [[nodiscard]] bool is_occupied(Point point) const;

private:
    void spawn_food();
    bool is_opposite(Direction a, Direction b) const;
    bool collides(Point point) const;
    void emit(EventType type, std::string message = {});

    int width_;
    int height_;
    std::vector<Point> snake_;
    Point food_{};
    Direction direction_{Direction::Right};
    int score_{};
    GameStatus status_{GameStatus::Menu};
    std::mt19937 random_;
};

class GameState;

class GameController {
public:
    explicit GameController(Game& game);
    ~GameController();
    GameController(const GameController&) = delete;
    GameController& operator=(const GameController&) = delete;

    void handle(char command);
    [[nodiscard]] GameStatus status() const noexcept;
    [[nodiscard]] Game& game() noexcept { return game_; }
    void change_state(std::unique_ptr<GameState> state);

private:
    Game& game_;
    std::unique_ptr<GameState> state_;
};

class GameState {
public:
    virtual ~GameState() = default;
    virtual void handle(GameController& controller, char command) = 0;
    [[nodiscard]] virtual GameStatus status() const noexcept = 0;
};

std::string status_name(GameStatus status);

} // namespace snake
