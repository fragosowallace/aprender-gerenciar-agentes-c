# 🗺️ Plano de Marcos (Milestones)

Cada marco vira uma ou mais **Issues**. A ordem importa: cada marco depende do anterior.

| #   | Marco            | Entrega (Definition of Done)                                             | Status |
|-----|------------------|-------------------------------------------------------------------------|--------|
| M0  | 🔧 Ambiente      | Toolchain (gcc + raylib + make) instalado; compila uma janela vazia     | ✅ Feito |
| M1  | 🪟 Janela + Loop | `src/main.c` abre janela 800×600 a 60fps com game loop e tela limpa      | ⬜     |
| M2  | 🕹️ Player        | Retângulo/sprite que anda com WASD/setas; câmera 2D (`Camera2D`) segue    | ⬜     |
| M3  | 🎨 Pixel art     | Carregar sprite do herói, render pixel-perfeito, animação idle/andar     | ⬜     |
| M4  | 👾 Inimigos      | Spawn periódico de inimigos que perseguem o player                       | ⬜     |
| M5  | ⚔️ Combate       | Projétil auto-disparo, colisão projétil↔inimigo, inimigo morre           | ⬜     |
| M6  | 💎 XP / Level    | Gemas de XP dropam, player coleta, sobe de nível, menu de upgrade        | ⬜     |
| M7  | 🔱 Armas         | 3–4 armas/passivas diferentes selecionáveis no level up                  | ⬜     |
| M8  | 📈 Dificuldade   | Ondas com o tempo, timer de sobrevivência, escalonamento de inimigos     | ⬜     |
| M9  | ❤️ HUD + Fim     | Barra de vida, relógio, contador de kills, tela de game over             | ⬜     |
| M10 | ✨ Juice + Arte  | Screenshake, partículas, sons, sprites definitivos                       | ⬜     |

## Princípios

- **Uma issue = uma tarefa pequena e revisável.** Se a descrição fica longa demais, quebre em duas.
- **Issues independentes** podem ser trabalhadas por agentes em paralelo (branches diferentes).
- **Todo código entra por Pull Request** que referencia a issue (`Closes #N`).
- **Definition of Done** clara em cada issue: como saber que terminou e como testar.
