#include "snake/game.hpp"
#include <cctype>

namespace snake {
namespace {
class MenuState;
class PlayingState;
class PausedState;
class GameOverState;

class MenuState final : public GameState {
public: void handle(GameController&, char) override;
    GameStatus status() const noexcept override { return GameStatus::Menu; }
};
class PlayingState final : public GameState {
public: void handle(GameController&, char) override;
    GameStatus status() const noexcept override { return GameStatus::Playing; }
};
class PausedState final : public GameState {
public: void handle(GameController&, char) override;
    GameStatus status() const noexcept override { return GameStatus::Paused; }
};
class GameOverState final : public GameState {
public: void handle(GameController&, char) override;
    GameStatus status() const noexcept override { return GameStatus::GameOver; }
};

char lower(char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); }

void MenuState::handle(GameController& controller, char command) {
    if (command == '\n' || command == '\r' || command == ' ') {
        controller.game().start();
        controller.change_state(std::make_unique<PlayingState>());
    } else if (lower(command) == 'q') controller.game().set_status(GameStatus::Exit, "Выход из игры");
}
void PlayingState::handle(GameController& controller, char command) {
    Game& game = controller.game();
    switch (lower(command)) {
        case 'p': game.set_status(GameStatus::Paused, "Игра приостановлена"); controller.change_state(std::make_unique<PausedState>()); return;
        case 'q': game.set_status(GameStatus::Exit, "Выход из игры"); return;
        case 'w': game.turn(Direction::Up); break;
        case 's': game.turn(Direction::Down); break;
        case 'a': game.turn(Direction::Left); break;
        case 'd': game.turn(Direction::Right); break;
        case ' ': game.advance(); break;
        default: break;
    }
    if (game.status() == GameStatus::GameOver) controller.change_state(std::make_unique<GameOverState>());
}
void PausedState::handle(GameController& controller, char command) {
    if (lower(command) == 'p' || command == ' ') {
        controller.game().set_status(GameStatus::Playing, "Игра продолжена");
        controller.change_state(std::make_unique<PlayingState>());
    } else if (lower(command) == 'q') controller.game().set_status(GameStatus::Exit, "Выход из игры");
}
void GameOverState::handle(GameController& controller, char command) {
    if (lower(command) == 'r') {
        controller.game().start();
        controller.change_state(std::make_unique<PlayingState>());
    } else if (lower(command) == 'q') controller.game().set_status(GameStatus::Exit, "Выход из игры");
}
} // namespace

GameController::GameController(Game& game) : game_(game), state_(std::make_unique<MenuState>()) {}
GameController::~GameController() = default;
void GameController::handle(char command) {
    if (game_.status() == GameStatus::Exit) return;
    state_->handle(*this, command);
    if (game_.status() == GameStatus::GameOver && state_->status() != GameStatus::GameOver)
        change_state(std::make_unique<GameOverState>());
}
GameStatus GameController::status() const noexcept { return state_->status(); }
void GameController::change_state(std::unique_ptr<GameState> state) { state_ = std::move(state); }
} // namespace snake
