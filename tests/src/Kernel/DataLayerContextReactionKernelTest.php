<?php

namespace Drupal\Tests\context_datalayer\Kernel;

use Drupal\KernelTests\Core\Entity\EntityKernelTestBase;
use Drupal\context_datalayer\Plugin\ContextReaction\DataLayer;

/**
 * DataLayer Context Reaction Kernel Tests.
 *
 * @group context_datalayer
 */
class DataLayerContextReactionKernelTest extends EntityKernelTestBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = [
    'context',
    'datalayer',
    'context_datalayer',
  ];

  /**
   * Test execute().
   */
  public function testExecute() {
    // Define a plugin configuration.
    $configuration = [
      'data' => [
        [
          'key' => 'foo',
          'value' => 'bar',
          'type' => 'baz',
        ],
        [
          'key' => 'foobar',
          'value' => 'barbaz',
          'type' => 'bazqux',
        ],
      ],
    ];
    // Define expected data to be sent to datalayer_add().
    $expected = [
      'data' => [
        'foo' => 'bar',
        'foobar' => 'barbaz',
      ],
      'overwrite' => 0,
    ];
    // Create a mock of our plugin so that we can use mock facilities.
    $plugin = new DataLayer($configuration, 'datalayer', '');
    // Invoke execute().
    $this->assertEquals($expected, $plugin->execute());
  }

}
