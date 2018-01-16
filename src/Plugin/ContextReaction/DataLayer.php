<?php

namespace Drupal\context_datalayer\Plugin\ContextReaction;

use Drupal\context\ContextReactionPluginBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * DataLayer Context Reaction Plugin.
 *
 * @ContextReaction(
 *   id = "datalayer",
 *   label = @Translation("DataLayer")
 * )
 */
class DataLayer extends ContextReactionPluginBase {

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return parent::defaultConfiguration() + [
      'data' => [],
      'overwrite' => 0,
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function summary() {
    return $this->t('Populates dataLayer with key/value pairs.');
  }

  /**
   * {@inheritdoc}
   */
  public function execute() {
    $config = $this->getConfiguration();
    $data = [];
    foreach ($config['data'] as $item) {
      $data[$item['key']] = $item['value'];
    }
    return [
      'data' => $data,
      'overwrite' => $config['overwrite'],
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function buildConfigurationForm(array $form, FormStateInterface $form_state) {
    $form['datalayer_key'] = [
      '#title' => $this->t('Key'),
      '#type' => 'textfield',
      '#description' => $this->t('The key for the key/value pair'),
    ];

    $form['datalayer_value'] = [
      '#title' => $this->t('Value'),
      '#type' => 'textfield',
      '#description' => $this->t('The value for the key/value pair'),
    ];

    $form['datalayer_type'] = [
      '#title' => $this->t('Value type'),
      '#type' => 'select',
      '#description' => $this->t('The value type for the key/value pair'),
      '#options' => [
        'string' => $this->t('String'),
        'int' => $this->t('Integer'),
        'float' => $this->t('Float'),
        'boolean' => $this->t('Boolean'),
      ],
    ];

    $form['datalayer_overwrite'] = [
      '#title' => $this->t('Overwrite dataLayer'),
      '#type' => 'checkbox',
      '#description' => $this->t('Overwrite the existing dataLayer'),
    ];

    if (isset($this->getConfiguration()['data'][0])) {
      $config = $this->getConfiguration();
      $data = $config['data'][0];
      $form['datalayer_key']['#default_value'] = $data['key'];
      $form['datalayer_value']['#default_value'] = $data['value'];
      $form['datalayer_type']['#default_value'] = $data['type'];
      $form['datalayer_overwrite']['#default_value'] = $config['overwrite'];
    }

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitConfigurationForm(array &$form, FormStateInterface $form_state) {
    $this->setConfiguration([
      'data' => [
        [
          'key' => $form_state->getValue('datalayer_key'),
          'value' => $form_state->getValue('datalayer_value'),
          'type' => $form_state->getValue('datalayer_type'),
        ],
      ],
      'overwrite' => $form_state->getValue('datalayer_overwrite'),
    ]);
  }

}
