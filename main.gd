extends Control

# --- Nós da cena ---
@onready var noise_screen: TextureRect = $TelaRuido
@onready var slider_oitavas: HSlider = $VBoxContainer/HSliderOitavas
@onready var slider_persistencia: HSlider = $VBoxContainer/HSliderPersistencia
@onready var label_oitavas: Label = $VBoxContainer/LabelOitavas
@onready var label_persistencia: Label = $VBoxContainer/LabelPersistencia

# --- Parâmetros ajustáveis no Inspector (em vez de magic numbers no corpo do código) ---
@export var seed_ruido: int = 12345
@export var escala_amostragem: float = 0.01     # frequência base do ruído
@export var amplitude_saida: float = 12.0       # normalização do buffer de saída
@export var lacunaridade: float = 2.0           # padrão da literatura (Perlin/Musgrave); ajuste com cautela
@export var velocidade: float = 700.0           # px/s de deslocamento via WASD

const LARGURA_BUFFER: int = 800
const ALTURA_BUFFER: int = 600

@export var modo_fade: PerlinNoise.FadeType = PerlinNoise.FadeType.QUINTIC

var gerador_terreno: PerlinNoise
var offset_x: float = 0.0
var offset_y: float = 0.0


func _ready() -> void:
	if not ClassDB.class_exists("PerlinNoise"):
		push_error("Classe nativa 'PerlinNoise' não encontrada. Verifique se a GDExtension foi compilada e o .gdextension está registrado.")
		set_process(false)
		return

	gerador_terreno = PerlinNoise.new()
	gerador_terreno.seed = seed_ruido

	var altura_neste_ponto: float = gerador_terreno.sample(50.0, 120.0)
	if altura_neste_ponto > 0.6:
		print("Gerar montanha aqui")
	else:
		print("Gerar planície")

	slider_oitavas.value_changed.connect(_on_oitavas_alteradas)
	slider_persistencia.value_changed.connect(_on_persistencia_alterada)

	_atualizar_labels()
	_desenhar_ruido()


func _process(delta: float) -> void:
	if gerador_terreno == null:
		return

	var moveu: bool = false

	if Input.is_physical_key_pressed(KEY_W):
		offset_y -= velocidade * delta
		moveu = true
	if Input.is_physical_key_pressed(KEY_S):
		offset_y += velocidade * delta
		moveu = true
	if Input.is_physical_key_pressed(KEY_A):
		offset_x -= velocidade * delta
		moveu = true
	if Input.is_physical_key_pressed(KEY_D):
		offset_x += velocidade * delta
		moveu = true

	if moveu:
		_desenhar_ruido()


func _on_oitavas_alteradas(_novo_valor: float) -> void:
	_atualizar_labels()
	_desenhar_ruido()


func _on_persistencia_alterada(_novo_valor: float) -> void:
	_atualizar_labels()
	_desenhar_ruido()


func _atualizar_labels() -> void:
	label_oitavas.text = "Oitavas: %d" % snapped(slider_oitavas.value, 0)
	label_persistencia.text = "Persistência: %.2f" % snapped(slider_persistencia.value, 0.05)


func _desenhar_ruido() -> void:
	if gerador_terreno == null:
		return

	gerador_terreno.octaves = int(slider_oitavas.value)
	gerador_terreno.persistence = float(slider_persistencia.value)
	gerador_terreno.lacunarity = lacunaridade
	gerador_terreno.fade_mode = modo_fade

	var buffer: PackedByteArray = gerador_terreno.gerar_fbm_buffer(
		LARGURA_BUFFER, ALTURA_BUFFER,
		escala_amostragem, offset_x, offset_y,
		amplitude_saida
	)

	var img: Image = Image.create_from_data(LARGURA_BUFFER, ALTURA_BUFFER, false, Image.FORMAT_RGBA8, buffer)
	noise_screen.texture = ImageTexture.create_from_image(img)
