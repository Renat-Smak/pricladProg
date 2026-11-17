#include "snake/game.hpp"
#include <iostream>
#include <string>

namespace {
class ConsoleObserver final : public snake::Observer {
public:
    void on_event(const snake::GameEvent& event) override {
        if (!event.message.empty()) last_message_ = event.message;
    }
    const std::string& last_message() const { return last_message_; }
private:
    std::string last_message_;
};

void render(const snake::Game& game, snake::GameStatus state, const std::string& message) {
    std::cout << "\n\033[2J\033[H";
    std::cout << "========== ЗМЕЙКА | Вариант 10 ==========\n";
    std::cout << "Счёт: " << game.score() << "    Состояние: " << snake::status_name(state) << "\n";
    if (state == snake::GameStatus::Menu) {
        std::cout << "\nУправление: WASD — направление, пробел — шаг, P — пауза, Q — выход.\n";
        std::cout << "Нажмите Enter, чтобы начать.\n";
        return;
    }
    for (int y = 0; y < game.height(); ++y) {
        for (int x = 0; x < game.width(); ++x) {
            const snake::Point p{x, y};
            char cell = ' ';
            if (x == 0 || y == 0 || x == game.width()-1 || y == game.height()-1) cell = '#';
            if (p == game.food()) cell = '*';
            if (game.is_occupied(p)) cell = (p == game.snake().front() ? '@' : 'o');
            std::cout << cell;
        }
        std::cout << '\n';
    }
    std::cout << "\nW/A/S/D — направление | Пробел — шаг | P — пауза | Q — выход\n";
    if (state == snake::GameStatus::Paused) std::cout << "Пауза: P или пробел — продолжить; Q — выход.\n";
    if (state == snake::GameStatus::GameOver) std::cout << "R — начать заново | Q — выход.\n";
    if (!message.empty()) std::cout << message << '\n';
}
}

int main() {
    try {
        snake::Game game;
        ConsoleObserver observer;
        game.subscribe(observer);
        snake::GameController controller(game);
        render(game, controller.status(), {});
        while (game.status() != snake::GameStatus::Exit) {
            std::cout << "> " << std::flush;
            char command{};
            if (!std::cin.get(command)) break;
            if (command == '\n' && controller.status() == snake::GameStatus::Playing) continue;
            controller.handle(command);
            render(game, controller.status(), observer.last_message());
        }
        game.unsubscribe(observer);
        std::cout << "Спасибо за игру!\n";
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
