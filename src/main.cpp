#include "Application.hpp"

int main(void) {
    Application app;
    app.init();

    app.run();

    app.exit();
    return 0;
}
