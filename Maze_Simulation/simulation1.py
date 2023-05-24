import pygame
import math
import time
pygame.init()
# Grid settings
ROWS = 10
COLS = 10
CELL_SIZE = 70
visit = set()
queue = []
def dfs(r,c):
   
    if r == len(grid) or c == len(grid[0]) or min(r,c)<0 or grid[r][c]==RED or (r,c) in visit:
                return 0
    else:
        visit.add((r,c))
        grid[r][c]=(0,255,0)
        for row in range(ROWS):
            for col in range(COLS):
                pygame.draw.rect(window, grid[row][col], (col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE))
        grid[r][c]=(0,0,255)
        pygame.display.update()
        time.sleep(0.1) 
        dfs(r+1,c)
        dfs(r,c+1)
        dfs(r,c-1)
        dfs(r-1,c)
                
             
def bfs():
    
    queue.append((0, 0))
    visit.add((0, 0))
    while queue:
        
        for i in range(len(queue)):
            r, c = queue.pop(0)
            if r == len(grid) - 1 and c == len(grid[0]) - 1:
                return


            neighbors = [[0, 1], [1, 0], [-1, 0], [0, -1]]

            for dr, dc in neighbors:
                if min(r + dr, c + dc) < 0 or r + dr == len(grid) or c + dc == len(grid[0]) or grid[r + dr][
                    c + dc] == RED or (dr + r, dc + c) in visit:
                    continue
                visit.add((r + dr, c + dc))
                grid[r + dr][ c + dc]=(0,0,255)
                for row in range(ROWS):
                    for col in range(COLS):
                        pygame.draw.rect(window, grid[row][col], (col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE))
                pygame.display.update()
                time.sleep(0.1) 
                
                queue.append((r + dr, c + dc))
def nav(r,c):
    if r == len(grid) or c == len(grid[0]) or min(r,c)<0 or grid[r][c]==RED or (r,c) in visit:
                return 0
           
                
lineY=pygame.Surface((2, ROWS*CELL_SIZE))
lineX=pygame.Surface((ROWS*CELL_SIZE, 2))
lineY.fill('white')
lineX.fill('white')
def round_to_nearest_multiple(num, multiple):
    return math.ceil(num / multiple) * multiple
# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)

window_width = COLS * CELL_SIZE
window_height = ROWS * CELL_SIZE
window_size = (window_width, window_height)
window = pygame.display.set_mode(window_size)
pygame.display.set_caption("Grid")

grid = [[BLACK for _ in range(COLS)] for _ in range(ROWS)]

# move = pygame.K_SPACE
pygame.time.set_timer(pygame.ANYFORMAT,1000)
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            mouse_pos = pygame.mouse.get_pos()
            col = round_to_nearest_multiple(mouse_pos[0] , CELL_SIZE)/(CELL_SIZE)
            row = round_to_nearest_multiple(mouse_pos[1] , CELL_SIZE) /(CELL_SIZE)
            grid[int(row)-1][int(col)-1] = RED  # Change the color of the clicked cell to red
        elif event.type == pygame.KEYDOWN:
            if event.key  == pygame.K_SPACE:
                # bfs()
                dfs(0,0)


    window.fill((BLACK))  # Clear the window with a black background
    
    # Draw the grid
    for row in range(ROWS):
        for col in range(COLS):
            pygame.draw.rect(window, grid[row][col], (col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE))
    for i in range(CELL_SIZE,ROWS*CELL_SIZE,CELL_SIZE):
        window.blit(lineY, (i, 0))
    for i in range(CELL_SIZE,ROWS*CELL_SIZE,CELL_SIZE):
        window.blit(lineX,(0,i))
    pygame.time.Clock().tick(60)
    pygame.display.update()

   
            
            
        
        
        

pygame.quit()
