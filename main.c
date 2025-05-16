

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// -------------------------------
// [INFO]: edit bagian ini saja
// -------------------------------
#define NODE 8
#define EDGE 12
#define SIZE_RESULT 5
#define MAX_PERMUTASI 1000000000

int nodes_index[EDGE][2] = {
  { 0, 1 },
  { 0, 2 },
  { 0, 4 },
  { 1, 3 },
  { 1, 7 },
  { 2, 5 },
  { 2, 3 },
  { 4, 6 },
  { 4, 5 },
  { 5, 7 },
  { 6, 7 },
  { 6, 3 },
};

// ==

uint64_t max_combination;
uint8_t* bitset;
FILE* bitset_file;

uint64_t factorial(int n)
{
  uint64_t res = 1;
  for (int i = 1; i <= n; i++)
    res *= i;

  return res;
}

uint64_t permutasi(int n, int k)
{
  return factorial(n) / factorial(n - k);
}

uint64_t factorial_rank(int n)
{
  static uint64_t fact[NODE + 1] = { 1 };
  static int initialized = 0;
  if (!initialized) {
    for (int i = 1; i <= NODE; i++) {
      fact[i] = fact[i - 1] * i;
    }
    initialized = 1;
  }
  return fact[n];
}

uint64_t perm_to_rank(int perm[NODE])
{
  uint64_t rank = 0;
  uint8_t used[NODE] = { 0 };

  for (int i = 0; i < NODE; i++) {
    int smaller = 0;
    for (int j = 0; j < perm[i] - 1; j++) {
      if (!used[j])
        smaller++;
    }
    rank += smaller * factorial(NODE - 1 - i);
    used[perm[i] - 1] = 1;
  }

  return rank;
}

bool is_seen(uint64_t rank)
{
  return (bitset[rank / 8] & (1 << (rank % 8))) != 0;
}

void mark_seen(uint64_t rank)
{
  bitset[rank / 8] |= (1 << (rank % 8));
}

void init_bitset()
{
  int size = (permutasi(NODE, NODE) + 7) / 8;
  bitset = (uint8_t*)calloc(size, 1);
  if (bitset == NULL) {
    printf("Gagal mengalokasikan memori untuk bitset!\n");
    exit(1);
  }
}

void init_bitset_file(const char* filename, size_t total_bits)
{
  size_t bytes = (total_bits + 7) / 8;
  bitset_file = fopen(filename, "r+b");
  if (!bitset_file) {
    bitset_file = fopen(filename, "w+b");
    if (!bitset_file) {
      perror("Gagal membuka file bitset");
      exit(1);
    }
    uint8_t zero = 0;
    for (size_t i = 0; i < bytes; i++) {
      fwrite(&zero, 1, 1, bitset_file);
    }
  }
}

void free_bitset()
{
  free(bitset);
}

void set_empty_arr(int arr[], int size)
{
  for (int i = 0; i < size; i++) {
    arr[i] = 0;
  }
}

int generate_number()
{
  int min = 1;
  int max = NODE;

  return rand() % (max - min + 1) + min;
}

bool includes(int arr[], int size, int target)
{
  for (int i = 0; i < size; i++) {
    if (arr[i] == target) {
      return true;
    }
  }
  return false;
}

void shuffle(int* arr, int n)
{
  for (int i = n - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}

void generate_combination(int nodes[])
{
  for (int i = 0; i < NODE; i++) {
    nodes[i] = i + 1;
  }
  shuffle(nodes, NODE);
}

// void generate_combination(int nodes[])
// {
//   set_empty_arr(nodes, NODE);
//
//   for (int i = 0; i < NODE; i++) {
//     int buf = generate_number();
//     while (includes(nodes, NODE, buf)) {
//
//       buf = generate_number();
//     }
//     nodes[i] = buf;
//   }
// }

void sum_nodes(int nodes[], int calculate_nodes[])
{
  set_empty_arr(calculate_nodes, EDGE);
  int size = 0;

  for (int i = 0; i < EDGE; i++) {
    int res = 0;
    for (int j = 0; j < 2; j++) {
      res += nodes[nodes_index[i][j]];
    }
    calculate_nodes[i] = res;
  }
}

void bubble_sort(int arr[], int size)
{
  for (int i = 0; i < size - 1; i++) {
    for (int j = 0; j < size - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        int temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }
}

void display_arr(int arr[], int size)
{
  printf("\t: [ ");
  for (int i = 0; i < size; i++) {
    printf("%d", arr[i]);
    if (i < size - 1) {
      printf(", ");
    } else {
      printf(" ");
    }
  }
  printf("]\n");
}

void filter_result(int calculate_nodes[], int result[], int* size_result)
{
  *size_result = 0;

  for (int i = 0; i < EDGE; i++) {
    if (*size_result >= 100) {

      printf("buffer overflow\n");
      break;
    }
    if (!includes(result, *size_result, calculate_nodes[i])) {
      result[*size_result] = calculate_nodes[i];
      *size_result += 1;
    }
  }
}

void display_result(int node[], int calculate_nodes[], int short_calculate[], int result[], int size_result)
{
  printf("W found : %d\n", size_result);
  printf("\tcombination ");
  display_arr(node, NODE);

  printf("\tcalculate ");
  display_arr(calculate_nodes, EDGE);

  printf("\tsort calculate ");
  display_arr(short_calculate, EDGE);

  printf("\tresult \t  ");
  display_arr(result, size_result);
}

bool is_seen_file(uint64_t rank)
{
  size_t byte_index = rank / 8;
  int bit_index = rank % 8;

  fseek(bitset_file, byte_index, SEEK_SET);
  uint8_t byte;
  fread(&byte, 1, 1, bitset_file);

  return (byte >> bit_index) & 1;
}

void mark_seen_file(uint64_t rank)
{
  size_t byte_index = rank / 8;
  int bit_index = rank % 8;

  fseek(bitset_file, byte_index, SEEK_SET);
  uint8_t byte;
  fread(&byte, 1, 1, bitset_file);

  byte |= (1 << bit_index);

  fseek(bitset_file, byte_index, SEEK_SET);
  fwrite(&byte, 1, 1, bitset_file);
}
void close_bitset_file()
{
  fclose(bitset_file);
}

int main()
{

  srand(time(NULL));
  // init_bitset();

  time_t now = time(NULL);

  char filename[100];
  snprintf(filename, sizeof(filename), "bitset_%ld.dat", now);
  max_combination = permutasi(NODE, NODE);
  init_bitset_file(filename, MAX_PERMUTASI);

  bool stop_program = false;
  bool combination_found = false;

  int combination_count = 0;
  int result[10000] = {};

  int size_result = 0;
  int nodes[NODE] = {};
  int calculate_nodes[EDGE] = {};
  int short_calculate[EDGE] = {};

  while (combination_count < max_combination && !stop_program) {
    printf("Kombinasi ke\t: %d\n", combination_count + 1);

    bool valid = false;

    while (!valid) {
      generate_combination(nodes);

      uint64_t rank = perm_to_rank(nodes);

      if (!is_seen_file(rank)) {
        mark_seen_file(rank);
        combination_count++;
        valid = true;
      }

      // if (!is_seen(rank)) {
      //
      //   printf("5\n");
      //   mark_seen(rank);
      //   printf("6\n");
      //   combination_count++;
      //   valid = true;
      // }
    }

    sum_nodes(nodes, calculate_nodes);

    for (int i = 0; i < EDGE; i++) {
      short_calculate[i] = calculate_nodes[i];
    }

    bubble_sort(short_calculate, EDGE);

    filter_result(calculate_nodes, result, &size_result);

    if (combination_count >= max_combination) {
      stop_program = true;
    } else if (size_result <= SIZE_RESULT) {
      combination_found = true;
      stop_program = true;
    }
  }

  printf("\nKombinasi Maksimal : %lu\n", max_combination);
  printf("W Maxsimal : %d\n", SIZE_RESULT);

  if (combination_found) {
    printf("\n[Kombinasi Ditemukan]\n");
    display_result(nodes, calculate_nodes, short_calculate, result, size_result);
  } else
    printf("\n[Kombinasi Tidak Ditemukan]\n");

  // free_bitset();

  close_bitset_file();

  return 0;
}
