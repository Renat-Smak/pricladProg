#include "snake/game.hpp"
#include <iostream>
#include <stdexcept>

#define CHECK(expr) do { if (!(expr)) throw std::runtime_error(std::string("Проверка не пройдена: ") + #expr); } while (false)

struct Counter final : snake::Observer {
    int events{};
    void on_event(const snake::GameEvent&) override { ++events; }
};

int main() {
    using namespace snake;
    try {
        Game game(12, 10, 1234);
        Counter observer;
        game.subscribe(observer);
        game.start();
        CHECK(game.status() == GameStatus::Playing);
        CHECK(game.snake().size() == 3);
        const Point head = game.snake().front();
        CHECK(game.advance());
        const Point expected{head.x + 1, head.y};
        CHECK(game.snake().front() == expected);
        CHECK(game.snake().size() == 3);
        CHECK(observer.events >= 1);

        GameController controller(game);
        CHECK(controller.status() == GameStatus::Menu);
        controller.handle('\n');
        CHECK(controller.status() == GameStatus::Playing);
        controller.handle('p');
        CHECK(controller.status() == GameStatus::Paused);
        controller.handle('p');
        CHECK(controller.status() == GameStatus::Playing);

        for (int i = 0; i < 20 && game.status() == GameStatus::Playing; ++i) game.advance();
        CHECK(game.status() == GameStatus::GameOver);
        std::cout << "Все тесты пройдены.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
