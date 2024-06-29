#include <SDL.h>
#include <iostream>
#include <vector>

const int SZEROKOSC_EKRANU = 800;
const int WYSOKOSC_EKRANU = 600;

// Pozycja i kolor
struct Punkt {
    int x, y;
    Uint8 r, g, b;
};

// Przyciski koloru
class PrzyciskKoloru {
public:
    SDL_Rect prostokat; // przyciski
    Uint8 r, g, b; // kolory

    PrzyciskKoloru(SDL_Rect prostokat, Uint8 r, Uint8 g, Uint8 b) : prostokat(prostokat), r(r), g(g), b(b) {}

    // Sprawdzanie czy myszka znajduje sie na przyciskach koloru
    bool czyNacisniety(int mouseX, int mouseY) const {
        return (mouseX >= prostokat.x && mouseX <= prostokat.x + prostokat.w && mouseY >= prostokat.y && mouseY <= prostokat.y + prostokat.h);
    }
};

// Przycisk resetu
class PrzyciskResetu {
public:
    SDL_Rect prostokat;

    PrzyciskResetu(SDL_Rect prostokat) : prostokat(prostokat) {}

    // Sprawdzanie czy myszka znajduje sie na przycisku resetu
    bool czyNacisniety(int mouseX, int mouseY) const {
        return (mouseX >= prostokat.x && mouseX <= prostokat.x + prostokat.w && mouseY >= prostokat.y && mouseY <= prostokat.y + prostokat.h);
    }
};

int main(int argc, char* argv[]) {
    // SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Inicjalizacja SDL nie powiodła się: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Tworzenie okna
    SDL_Window* okno = SDL_CreateWindow("Rysowanie z SDL | Kruczek", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SZEROKOSC_EKRANU, WYSOKOSC_EKRANU, SDL_WINDOW_SHOWN);
    if (!okno) {
        std::cerr << "Tworzenie okna nie powiodło się: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Tworzenie renderera
    SDL_Renderer* renderer = SDL_CreateRenderer(okno, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Tworzenie renderera nie powiodło się: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(okno);
        SDL_Quit();
        return 1;
    }

    std::vector<Punkt> punkty;
    std::vector<PrzyciskKoloru> przyciskiKoloru = {
        { {10, 10, 50, 50}, 255, 0, 0 },
        { {70, 10, 50, 50}, 0, 255, 0 },
        { {130, 10, 50, 50}, 0, 0, 255 },
        { {190, 10, 50, 50}, 255, 255, 0 },
        { {250, 10, 50, 50}, 255, 0, 255 },
        { {310, 10, 50, 50}, 255, 255, 255 },
        { {SZEROKOSC_EKRANU - 110, 10, 100, 50}, 128, 128, 128 }  // Przycisk w prawym górnym rogu ( resetu )
    };

    Uint8 aktualnyKolorDomyslny[3] = {255, 255, 255};
    Uint8 aktualnyKolor[3] = {255, 255, 255};
    bool rysowanie = false;

    bool quit = false;
    SDL_Event e;

    // Główna pętla programu
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    rysowanie = true;

                    // Sprawdzanie, czy naciśnięto na któryś przycisk koloru
                    for (auto& przycisk : przyciskiKoloru) {
                        if (przycisk.czyNacisniety(e.button.x, e.button.y)) {
                            aktualnyKolor[0] = przycisk.r;
                            aktualnyKolor[1] = przycisk.g;
                            aktualnyKolor[2] = przycisk.b;
                        }
                    }

                    // Obsługa przycisku resetu
                    if (przyciskiKoloru.back().czyNacisniety(e.button.x, e.button.y)) {
                        punkty.clear();
                        aktualnyKolor[0] = aktualnyKolorDomyslny[0];
                        aktualnyKolor[1] = aktualnyKolorDomyslny[1];
                        aktualnyKolor[2] = aktualnyKolorDomyslny[2];
                    }
                }
            } else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    rysowanie = false;
                }
            } else if (e.type == SDL_MOUSEMOTION) {
                if (rysowanie) {
                    Punkt p = {e.motion.x, e.motion.y, aktualnyKolor[0], aktualnyKolor[1], aktualnyKolor[2]};
                    punkty.push_back(p);
                }
            }
        }

        // Czyszczenie
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Przyciski kolor
        for (const auto& przycisk : przyciskiKoloru) {
            SDL_SetRenderDrawColor(renderer, przycisk.r, przycisk.g, przycisk.b, 255);
            SDL_RenderFillRect(renderer, &przycisk.prostokat);
        }

        // Rysowanie
        for (const auto& punkt : punkty) {
            SDL_SetRenderDrawColor(renderer, punkt.r, punkt.g, punkt.b, 255);
            SDL_RenderDrawPoint(renderer, punkt.x, punkt.y);
        }

        // Odświeżenie renderera
        SDL_RenderPresent(renderer);
    }

    // Zamykanie i czyszczenie
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(okno);
    SDL_Quit();

    return 0;
}
