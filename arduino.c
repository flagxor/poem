static cell_t dstack[256], rstack[256], user[256], heap[1024 * 4];
static cell_t *dsp = dstack;
static cell_t *rsp = rstack;
static cell_t *ip = 0;
char buffer[256];
char *pos = buffer;

static void terminate(cell_t x) {
}

static void emit(cell_t x) {
  Serial.write(x);
}

void setup() {
  Serial.begin(9600);
  forth(
      boot_fs, boot_fs_len,
      &ip, &dsp, &rsp, user, heap, emit, terminate);
}

void loop() {
  int ch;

  if (!Serial.available()) {
    return;
  }
  ch = Serial.read();
  if (ch == '\n' || ch == '\r') {
    *pos = '\0';
    pos = buffer;
  } else {
    *pos = ch;
    ++pos;
    return;
  }
  strcat(buffer, " yield\n");
  forth(
      buffer, strlen(buffer),
      &ip, &dsp, &rsp, user, heap, emit, terminate);
}
