# Makefile

# Répertoires
BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = src
INCLUDE_DIR = includes
LD_FLAGS = -lpthread
# Listes des programmes à générer sans prefixe
PROGLIST= server app
PROGRAMMES=$(addprefix $(BIN_DIR)/, $(PROGLIST))


all: $(PROGRAMMES)
	@echo "Compilation terminee"


# Regle de compilation .c -> .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@gcc -c $< -o $@ -I$(INCLUDE_DIR) 

# Regle de compilation .o -> .exe
$(BIN_DIR)/server: $(OBJ_DIR)/server.o $(OBJ_DIR)/libInet.a $(OBJ_DIR)/mysyscall.o
	@mkdir -p $(BIN_DIR)
	@echo "Edition de liens de $<"
	@gcc $^ -o $@ $(LD_FLAGS) -g -I$(INCLUDE_DIR) -L$(OBJ_DIR) -lInet
	@echo "Creation d'un lien symbolique vers $@"
	@ln -sf $@ .

$(BIN_DIR)/app: $(OBJ_DIR)/app.o $(OBJ_DIR)/libInet.a $(OBJ_DIR)/mysyscall.o $(OBJ_DIR)/libavalapp.a
	@mkdir -p $(BIN_DIR)
	@echo "Edition de liens de $<"
	@gcc $^ -o $@ $(LD_FLAGS) -g -I$(INCLUDE_DIR) -L$(OBJ_DIR) -lInet -lavalapp
	@echo "Creation d'un lien symbolique vers $@"
	@ln -sf $@ .

# Regle pour les librairies
$(OBJ_DIR)/avalam.o: $(SRC_DIR)/avalam.c $(INCLUDE_DIR)/avalam.h $(INCLUDE_DIR)/topologie.h
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@gcc -c $< -o $@ -I$(INCLUDE_DIR) 

$(OBJ_DIR)/session.o: $(SRC_DIR)/session.c $(INCLUDE_DIR)/session.h
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@gcc -c $< -o $@ -I$(INCLUDE_DIR) -DSESSION_DEBUG

$(OBJ_DIR)/data.o: $(SRC_DIR)/data.c $(INCLUDE_DIR)/data.h
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@gcc -c $< -o $@ -I$(INCLUDE_DIR) -DDATA_DEBUG

$(OBJ_DIR)/aotp.o: $(SRC_DIR)/aotp.c $(INCLUDE_DIR)/aotp.h
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@gcc -c $< -o $@ -I$(INCLUDE_DIR) -DAOTP_DEBUG

$(OBJ_DIR)/design.o: $(SRC_DIR)/design.c $(INCLUDE_DIR)/design.h
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@gcc -c $< -o $@ -I$(INCLUDE_DIR)

$(OBJ_DIR)/req.o: $(SRC_DIR)/req.c $(INCLUDE_DIR)/req.h
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@gcc -c $< -o $@ -I$(INCLUDE_DIR)

$(OBJ_DIR)/libavalapp.a: $(OBJ_DIR)/design.o $(OBJ_DIR)/req.o
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@ar -crs $@ $^

$(OBJ_DIR)/client.o: $(SRC_DIR)/client.c $(INCLUDE_DIR)/client.h
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@gcc -c $< -o $@ -I$(INCLUDE_DIR)

$(OBJ_DIR)/party.o: $(SRC_DIR)/party.c $(INCLUDE_DIR)/party.h
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@gcc -c $< -o $@ -I$(INCLUDE_DIR)

$(OBJ_DIR)/libInet.a: $(OBJ_DIR)/session.o $(OBJ_DIR)/avalam.o $(OBJ_DIR)/data.o $(OBJ_DIR)/aotp.o $(OBJ_DIR)/client.o $(OBJ_DIR)/party.o
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@ar -crs $@ $^

$(OBJ_DIR)/mysyscall.o: $(SRC_DIR)/mysyscall.c $(INCLUDE_DIR)/mysyscall.h
	@mkdir -p $(OBJ_DIR)
	@echo "Compilation de $<"
	@gcc -c $< -o $@ -I$(INCLUDE_DIR) -DSYS_DEBUG

doc:
	@echo "Creation de la documentation"
	@mkdir -p docs
	@doxygen DoxyFile
	
# Clean
clean:
	@echo "Suppression des fichiers objets et des executables"
	@rm -rf $(BIN_DIR)/*
	@rm -rf $(OBJ_DIR)/*
	@rm -rf server app
	@rm -rf docs/*

