from queue import PriorityQueue
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
        print(r,c)
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

                
             
def dijkstras_algorithm(matrix, source):
    rows = len(matrix)
    cols = len(matrix[0])

    # Initialize distance array with infinity for all cells except the source
    distance = [[float("inf") for _ in range(cols)] for _ in range(rows)]
    distance[source[0]][source[1]] = 0

    # Initialize parent array to store the parent of each cell in the shortest path
    parent = [[None for _ in range(cols)] for _ in range(rows)]

    # Define the possible directions to move (up, down, left, right)
    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]

    # Create a priority queue to store cells based on their distance from the source
    pq = PriorityQueue()
    pq.put((0, source))  # Insert the source cell with distance 0

    while not pq.empty():
        current_dist, current_cell = pq.get()
        current_row, current_col = current_cell

        # If the current cell has already been visited, skip it
        if current_dist > distance[current_row][current_col]:
            continue

        # Explore all possible neighbors
        for direction in directions:
            new_row = current_row + direction[0]
            new_col = current_col + direction[1]

            # Check if the new cell is within the matrix bounds
            if 0 <= new_row < rows and 0 <= new_col < cols:
                # Check if the new cell is an obstacle (1)
                if matrix[new_row][new_col] == RED:
                    continue

                # Calculate the distance to the new cell
                dist_to_new = current_dist + 1

                # Update the distance and parent if it's shorter than the current recorded distance
                if dist_to_new < distance[new_row][new_col]:
                    distance[new_row][new_col] = dist_to_new
                    parent[new_row][new_col] = current_cell
                    pq.put((dist_to_new, (new_row, new_col)))

    return parent

def get_shortest_path(parent, source, destination):
    path = []
    current = destination
    while current != source:
        path.append(current)
        current = parent[current[0]][current[1]]
    path.append(source)
    path.reverse()
    return path

def get_coordinates(shortest_path):
    return [(cell[0], cell[1]) for cell in shortest_path]
           
                
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
                parent = dijkstras_algorithm(grid, (0,0))
                shortest_path = get_shortest_path(parent, (0,0), (9,9))
                coordinates = get_coordinates(shortest_path)
                for r,c in coordinates:
                    grid[r][c]=(0,255,0)
                    for row in range(ROWS):
                        for col in range(COLS):
                            pygame.draw.rect(window, grid[row][col], (col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE))


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
