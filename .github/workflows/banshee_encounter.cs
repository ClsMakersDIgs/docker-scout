using UnityEngine;
using System.Collections;

public class BansheeAI : MonoBehaviour
{
    [Header("References")]
    [SerializeField] private GameObject lilithPrefab;
    [SerializeField] private GameObject dicePrefab;
    [SerializeField] private Transform lilithSpawnPoint;
    [SerializeField] private Transform diceSpawnPoint;
    [SerializeField] private GameObject playerObject;
    [SerializeField] private ParticleSystem mistEffect;
    
    [Header("Detection Settings")]
    [SerializeField] private float detectionRange = 10f;
    [SerializeField] private LayerMask playerLayer;
    
    [Header("Dice Behavior")]
    [SerializeField] private bool diceOrbiting = true; // true = orbiting, false = floating on ground
    [SerializeField] private float orbitSpeed = 50f;
    [SerializeField] private float orbitRadius = 0.5f;
    [SerializeField] private float floatHeight = 1f;
    [SerializeField] private float floatBobSpeed = 1f;
    [SerializeField] private float floatBobAmount = 0.2f;
    
    [Header("Graveyard Transformation")]
    [SerializeField] private GameObject normalWorld;
    [SerializeField] private GameObject graveyardWorld;
    [SerializeField] private float mistRiseDuration = 2f;
    [SerializeField] private float transformationDelay = 1f;
    [SerializeField] private float mistFallDuration = 2f;
    
    private bool encounterTriggered = false;
    private GameObject spawnedLilith;
    private GameObject[] spawnedDice;
    
    void Start()
    {
        // Find player if not assigned
        if (playerObject == null)
        {
            playerObject = GameObject.FindGameObjectWithTag("Player");
        }
        
        // Setup graveyard as inactive initially
        if (graveyardWorld != null)
        {
            graveyardWorld.SetActive(false);
        }
        
        // Setup mist as inactive
        if (mistEffect != null)
        {
            mistEffect.Stop();
        }
    }
    
    void Update()
    {
        if (!encounterTriggered && playerObject != null)
        {
            float distanceToPlayer = Vector3.Distance(transform.position, playerObject.transform.position);
            
            if (distanceToPlayer <= detectionRange)
            {
                encounterTriggered = true;
                StartCoroutine(TriggerEncounter());
            }
        }
    }
    
    IEnumerator TriggerEncounter()
    {
        // Summon Lilith
        yield return StartCoroutine(SummonLilith());
        
        // Lilith speaks riddle
        yield return StartCoroutine(LilithRiddle());
        
        // Mist rises
        yield return StartCoroutine(RaiseMist());
        
        // Transform world to graveyard
        yield return StartCoroutine(TransformToGraveyard());
        
        // Mist falls
        yield return StartCoroutine(LowerMist());
        
        // Banshee disappears
        DisappearBanshee();
        
        // Spawn the magic dice
        SpawnMagicDice();
    }
    
    IEnumerator SummonLilith()
    {
        if (lilithPrefab != null && lilithSpawnPoint != null)
        {
            spawnedLilith = Instantiate(lilithPrefab, lilithSpawnPoint.position, lilithSpawnPoint.rotation);
            
            // Optional: Add summoning effect here
            Debug.Log("Lilith has been summoned!");
            
            yield return new WaitForSeconds(1f);
        }
        else
        {
            Debug.LogWarning("Lilith prefab or spawn point not assigned!");
            yield return null;
        }
    }
    
    IEnumerator LilithRiddle()
    {
        // Display riddle - integrate with your dialogue system
        string riddle;
        
        if (diceOrbiting)
        {
            riddle = "Two fates entwined, forever spinning round,\n" +
                    "In endless dance above the hallowed ground.\n" +
                    "Seek the orbiting spheres that hold your destiny,\n" +
                    "Where chance and choice merge into unity.";
        }
        else
        {
            riddle = "Upon the earth yet not quite touching stone,\n" +
                    "Two cubes of power rest but not alone.\n" +
                    "Floating on the ground where shadows play,\n" +
                    "The dice of fate await to light your way.";
        }
        
        Debug.Log("Lilith speaks: " + riddle);
        
        // TODO: Replace with your dialogue system
        // Example: DialogueManager.Instance.ShowDialogue("Lilith", riddle);
        
        yield return new WaitForSeconds(5f); // Time to read riddle
    }
    
    IEnumerator RaiseMist()
    {
        if (mistEffect != null)
        {
            mistEffect.Play();
            Debug.Log("Mist rises...");
        }
        
        yield return new WaitForSeconds(mistRiseDuration);
    }
    
    IEnumerator TransformToGraveyard()
    {
        yield return new WaitForSeconds(transformationDelay);
        
        if (normalWorld != null)
        {
            normalWorld.SetActive(false);
        }
        
        if (graveyardWorld != null)
        {
            graveyardWorld.SetActive(true);
        }
        
        Debug.Log("World transformed to graveyard...");
    }
    
    IEnumerator LowerMist()
    {
        yield return new WaitForSeconds(mistFallDuration);
        
        if (mistEffect != null)
        {
            mistEffect.Stop();
            Debug.Log("Mist falls...");
        }
    }
    
    void DisappearBanshee()
    {
        // Optional: Add disappearing effect before destroying
        Debug.Log("Banshee leaves this realm...");
        
        // Destroy Lilith too
        if (spawnedLilith != null)
        {
            Destroy(spawnedLilith, 1f);
        }
        
        Destroy(gameObject, 1f);
    }
    
    void SpawnMagicDice()
    {
        if (dicePrefab == null || diceSpawnPoint == null)
        {
            Debug.LogWarning("Dice prefab or spawn point not assigned!");
            return;
        }
        
        if (diceOrbiting)
        {
            // Spawn two dice that orbit each other
            spawnedDice = new GameObject[2];
            
            GameObject dice1 = Instantiate(dicePrefab, diceSpawnPoint.position, Quaternion.identity);
            GameObject dice2 = Instantiate(dicePrefab, diceSpawnPoint.position, Quaternion.identity);
            
            spawnedDice[0] = dice1;
            spawnedDice[1] = dice2;
            
            // Add orbiting behavior
            OrbitingDice orbitScript = diceSpawnPoint.gameObject.AddComponent<OrbitingDice>();
            orbitScript.dice1 = dice1.transform;
            orbitScript.dice2 = dice2.transform;
            orbitScript.orbitSpeed = orbitSpeed;
            orbitScript.orbitRadius = orbitRadius;
            orbitScript.centerPoint = diceSpawnPoint.position;
            
            Debug.Log("Magic dice appear, orbiting each other!");
        }
        else
        {
            // Spawn dice floating on the ground
            GameObject dice = Instantiate(dicePrefab, diceSpawnPoint.position + Vector3.up * floatHeight, Quaternion.identity);
            spawnedDice = new GameObject[] { dice };
            
            // Add floating behavior
            FloatingDice floatScript = dice.AddComponent<FloatingDice>();
            floatScript.bobSpeed = floatBobSpeed;
            floatScript.bobAmount = floatBobAmount;
            floatScript.startHeight = floatHeight;
            
            Debug.Log("Magic dice appear, floating near the ground!");
        }
    }
    
    void OnDrawGizmosSelected()
    {
        // Visualize detection range
        Gizmos.color = Color.yellow;
        Gizmos.DrawWireSphere(transform.position, detectionRange);
    }
}

// Separate component for orbiting dice behavior
public class OrbitingDice : MonoBehaviour
{
    public Transform dice1;
    public Transform dice2;
    public float orbitSpeed = 50f;
    public float orbitRadius = 0.5f;
    public Vector3 centerPoint;
    
    private float angle = 0f;
    
    void Update()
    {
        if (dice1 == null || dice2 == null) return;
        
        angle += orbitSpeed * Time.deltaTime;
        
        // Position dice opposite each other
        float rad1 = angle * Mathf.Deg2Rad;
        float rad2 = (angle + 180f) * Mathf.Deg2Rad;
        
        dice1.position = centerPoint + new Vector3(
            Mathf.Cos(rad1) * orbitRadius,
            0,
            Mathf.Sin(rad1) * orbitRadius
        );
        
        dice2.position = centerPoint + new Vector3(
            Mathf.Cos(rad2) * orbitRadius,
            0,
            Mathf.Sin(rad2) * orbitRadius
        );
        
        // Optional: Rotate the dice themselves
        dice1.Rotate(Vector3.up, orbitSpeed * Time.deltaTime);
        dice2.Rotate(Vector3.up, orbitSpeed * Time.deltaTime);
    }
}

// Separate component for floating dice behavior
public class FloatingDice : MonoBehaviour
{
    public float bobSpeed = 1f;
    public float bobAmount = 0.2f;
    public float startHeight = 1f;
    
    private Vector3 startPosition;
    private float timeOffset;
    
    void Start()
    {
        startPosition = transform.position;
        timeOffset = Random.Range(0f, 2f * Mathf.PI);
    }
    
    void Update()
    {
        float newY = startPosition.y + Mathf.Sin(Time.time * bobSpeed + timeOffset) * bobAmount;
        transform.position = new Vector3(startPosition.x, newY, startPosition.z);
        
        // Optional: Gentle rotation
        transform.Rotate(Vector3.up, 20f * Time.deltaTime);
    }
}