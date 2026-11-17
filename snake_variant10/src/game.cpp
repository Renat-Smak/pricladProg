#include "snake/game.hpp"

#include <algorithm>
#include <stdexcept>

namespace snake {

void Subject::subscribe(Observer& observer) {
    if (std::find(observers_.begin(), observers_.end(), &observer) == observers_.end())
        observers_.push_back(&observer);
}

void Subject::unsubscribe(Observer& observer) {
    std::erase(observers_, &observer);
}

void Subject::notify(const GameEvent& event) {
    const auto snapshot = observers_;
    for (Observer* observer : snapshot) {
        if (observer && std::find(observers_.begin(), observers_.end(), observer) != observers_.end())
            observer->on_event(event);
    }
}

Game::Game(int width, int height, std::uint32_t seed)
    : width_(width), height_(height), random_(seed == 0 ? std::random_device{}() : seed) {
    if (width < 8 || height < 8) throw std::invalid_argument("Размер поля должен быть не меньше 8x8");
    reset();
}

void Game::reset() {
    snake_ = {{width_ / 2, height_ / 2}, {width_ / 2 - 1, height_ / 2}, {width_ / 2 - 2, height_ / 2}};
    direction_ = Direction::Right;
    score_ = 0;
    spawn_food();
}

void Game::start() {
    reset();
    set_status(GameStatus::Playing, "Игра началась");
}

bool Game::is_occupied(Point point) const {
    return std::find(snake_.begin(), snake_.end(), point) != snake_.end();
}

void Game::spawn_food() {
    std::vector<Point> free_cells;
    for (int y = 1; y < height_ - 1; ++y)
        for (int x = 1; x < width_ - 1; ++x)
            if (!is_occupied({x, y})) free_cells.push_back({x, y});
    if (free_cells.empty()) {
        set_status(GameStatus::GameOver, "Победа! Поле полностью заполнено");
        return;
    }
    std::uniform_int_distribution<std::size_t> distribution(0, free_cells.size() - 1);
    food_ = free_cells[distribution(random_)];
}

bool Game::is_opposite(Direction a, Direction b) const {
    return (a == Direction::Up && b == Direction::Down) ||
           (a == Direction::Down && b == Direction::Up) ||
           (a == Direction::Left && b == Direction::Right) ||
           (a == Direction::Right && b == Direction::Left);
}

bool Game::collides(Point point) const {
    if (point.x <= 0 || point.x >= width_ - 1 || point.y <= 0 || point.y >= height_ - 1) return true;
    // При движении на освободившуюся клетку хвоста столкновения нет.
    return std::find(snake_.begin(), snake_.end() - 1, point) != snake_.end() - 1;
}

bool Game::turn(Direction direction) {
    if (status_ != GameStatus::Playing || is_opposite(direction_, direction)) return false;
    direction_ = direction;
    return advance();
}

bool Game::advance() {
    if (status_ != GameStatus::Playing) return false;
    Point next = snake_.front();
    switch (direction_) {
        case Direction::Up: --next.y; break;
        case Direction::Down: ++next.y; break;
        case Direction::Left: --next.x; break;
        case Direction::Right: ++next.x; break;
    }
    if (collides(next)) {
        set_status(GameStatus::GameOver, "Столкновение! Игра окончена");
        emit(EventType::Collision, "Змейка столкнулась с препятствием");
        return false;
    }
    snake_.insert(snake_.begin(), next);
    if (next == food_) {
        score_ += 10;
        emit(EventType::FoodEaten, "Съедена еда: +10 очков");
        emit(EventType::ScoreChanged, "Счёт обновлён");
        spawn_food();
    } else {
        snake_.pop_back();
    }
    return true;
}

void Game::set_status(GameStatus status, std::string message) {
    if (status_ == status) return;
    status_ = status;
    emit(EventType::StateChanged, std::move(message));
}

void Game::emit(EventType type, std::string message) {
    notify(GameEvent{type, status_, score_, std::move(message)});
}

std::string status_name(GameStatus status) {
    switch (status) {
        case GameStatus::Menu: return "Меню";
        case GameStatus::Playing: return "Игра";
        case GameStatus::Paused: return "Пауза";
        case GameStatus::GameOver: return "Конец игры";
        case GameStatus::Exit: return "Выход";
    }
    return "Неизвестно";
}

} // namespace snake
